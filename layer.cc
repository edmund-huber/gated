#include <SDL.h>

#include "error.h"
#include "entity.h"

typedef enum {
    LEVEL_EMPTY,
    LEVEL_FLOAT,
    LEVEL_0 = 0,
    LEVEL_1 = 1
} level_t;

class Device {
public:
    virtual bool act(level_t *, level_t *) { return false; }
    static const uint8_t n_inputs, n_outputs;
    static const bool named_inputs;
};

class Cell {
public:
    level_t traces[4] = { LEVEL_EMPTY, LEVEL_EMPTY, LEVEL_EMPTY, LEVEL_EMPTY };
    Device *device = NULL;
};

typedef struct layer_coord {
    int16_t u, v;

    bool operator<(const struct layer_coord &o) const {
        return tie(u, v) < tie(o.u, o.v);
    }
} layer_coord_t;

class AndGate : public Device {
    virtual bool act(level_t *in, level_t *out) {
        // todo: something else checks that all input are not floating, and all output are floating?
        if (in[0] == LEVEL_1 && in[1] == LEVEL_1) {
            out[0] = LEVEL_1;
        } else {
            out[0] = LEVEL_0;
        }
        return true;
    }
    static const uint8_t n_inputs = 2, n_outputs = 1;
    static const bool named_inputs = false;
};

class OrGate : public Device {
    virtual bool act(level_t *in, level_t *out) {
        // todo: something else checks that all input are not floating, and all output are floating?
        if (in[0] == LEVEL_1 || in[1] == LEVEL_1) {
            out[0] = LEVEL_1;
        } else {
            out[0] = LEVEL_0;
        }
        return true;
    }
    static const uint8_t n_inputs = 2, n_outputs = 1;
    static const bool named_inputs = false;
};

AndGate and_gate;
OrGate or_gate;

LayerEntity::LayerEntity(Realm *r) : Entity(r) {
    offset_x = 0;
    offset_y = 0;
    highlight_u = 3;
    highlight_v = 4;
    zoom = 1;
}

void LayerEntity::draw(SDL_Renderer *renderer, SDL_DisplayMode &mode) {
    int16_t
        inner_size = zoom * 16,
        margin = zoom,
        size = inner_size + margin,
        device_size = zoom * 10;
    int16_t min_x = (offset_x - size) / size;
    for (int16_t x = min_x; (x * size) - offset_x < mode.w; x++) {
        int min_y = (offset_y - size) / size;
        for (int16_t y = min_y; (y * size) - offset_y < mode.h; y++) {
            // Draw the square,
            if (x == highlight_u && y == highlight_v) {
                CHECK_SDL_RET(SDL_SetRenderDrawColor(renderer, 0xff, 0, 0, 0xff));
            } else {
                CHECK_SDL_RET(SDL_SetRenderDrawColor(renderer, 0xaa, 0, 0, 0xff));
            }
            SDL_Rect rect = {
                .x = (x * size) - offset_x,
                .y = (y * size) - offset_y,
                .w = inner_size,
                .h = inner_size
            };
            CHECK_SDL_RET(SDL_RenderFillRect(renderer, &rect));

            layer_coord uv = { .u = x, .v = y };
            auto it = cells.find(uv);
            if (it != cells.end()) {
                Cell &cell = it->second;

                // draw the traces,
                CHECK_SDL_RET(SDL_SetRenderDrawColor(renderer, 0, 0xff, 0, 0xff));
                if (cell.traces[0] != LEVEL_EMPTY) {
                    SDL_Rect rect = {
                        .x = (x * size) + (inner_size / 2) - (inner_size / 16) - offset_x,
                        .y = (y * size) - margin - offset_y,
                        .w = (inner_size / 8),
                        .h = (inner_size / 2) + (inner_size / 16) + margin
                    };
                    CHECK_SDL_RET(SDL_RenderFillRect(renderer, &rect));
                }
                if (cell.traces[1] != LEVEL_EMPTY) {
                    SDL_Rect rect = {
                        .x = (x * size) + (inner_size / 2) - (inner_size / 16) - offset_x,
                        .y = (y * size) + (inner_size / 2) - (inner_size / 16) - offset_y,
                        .w = (inner_size / 2) + (inner_size / 16) + margin,
                        .h = inner_size / 8
                    };
                    CHECK_SDL_RET(SDL_RenderFillRect(renderer, &rect));
                }
                if (cell.traces[2] != LEVEL_EMPTY) {
                    SDL_Rect rect = {
                        .x = (x * size) + (inner_size / 2) - (inner_size / 16) - offset_x,
                        .y = (y * size) + (inner_size / 2) - (inner_size / 16) - offset_y,
                        .w = inner_size / 8,
                        .h = (inner_size / 2) + (inner_size / 16) + margin
                    };
                    CHECK_SDL_RET(SDL_RenderFillRect(renderer, &rect));
                }
                if (cell.traces[3] != LEVEL_EMPTY) {
                    SDL_Rect rect = {
                        .x = (x * size) - margin - offset_x,
                        .y = (y * size) + (inner_size / 2) - (inner_size / 16) - offset_y,
                        .w = (inner_size / 2) + (inner_size / 16) + margin,
                        .h = inner_size / 8
                    };
                    CHECK_SDL_RET(SDL_RenderFillRect(renderer, &rect));
                }

                // draw the device.
                if (cell.device != NULL) {
                    if (cell.device == &and_gate) {
                        CHECK_SDL_RET(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff));
                    } else if (cell.device == &or_gate) {
                        CHECK_SDL_RET(SDL_SetRenderDrawColor(renderer, 0, 0, 0xff, 0xff));
                    }
                    SDL_Rect rect = {
                        .x = (x * size) + (inner_size / 2) - (device_size / 2) - offset_x,
                        .y = (y * size) + (inner_size / 2) - (device_size / 2) - offset_y,
                        .w = device_size,
                        .h = device_size
                    };
                    CHECK_SDL_RET(SDL_RenderFillRect(renderer, &rect));
                }
            }
        }
    }

    CHECK_SDL_RET(SDL_SetRenderDrawColor(renderer, 0, 0, 0xff, 0xff));
    SDL_Rect rect = { .x = -offset_x, .y = -offset_y, .w = 32, .h = 32 };
    CHECK_SDL_RET(SDL_RenderFillRect(renderer, &rect));
}

void LayerEntity::draw_input_pixels(SDL_Renderer *renderer, SDL_DisplayMode &mode) {
    SDL_Rect rect = { .x = 0, .y = 0, .w = mode.w, .h = mode.h };
    CHECK_SDL_RET(SDL_RenderFillRect(renderer, &rect));
}

void LayerEntity::keyboard(SDL_KeyboardEvent &keyboard_event) {
    if (keyboard_event.type == SDL_KEYDOWN) {
        layer_coord highlight_uv = { .u = highlight_u, .v = highlight_v };
        switch (keyboard_event.keysym.sym) {
        case SDLK_w:
            offset_y -= 20;
            break;
        case SDLK_a:
            offset_x -= 20;
            break;
        case SDLK_s:
            offset_y += 20;
            break;
        case SDLK_d:
            offset_x += 20;
            break;
        case SDLK_z:
            zoom += 1;
            break;
        case SDLK_x:
            if (zoom > 1) {
                zoom -= 1;
            }
            break;
        case SDLK_j:
            highlight_u -= 1;
            break;
        case SDLK_l:
            highlight_u += 1;
            break;
        case SDLK_i:
            highlight_v -= 1;
            break;
        case SDLK_k:
            highlight_v += 1;
            break;
        #define SWITCH_TRACE(I) \
            if (cells[highlight_uv].traces[I] == LEVEL_EMPTY) { \
                cells[highlight_uv].traces[I] = LEVEL_FLOAT; \
            } else { \
                cells[highlight_uv].traces[I] = LEVEL_EMPTY; \
            }
        case SDLK_t:
            SWITCH_TRACE(0);
            break;
        case SDLK_h:
            SWITCH_TRACE(1);
            break;
        case SDLK_g:
            SWITCH_TRACE(2);
            break;
        case SDLK_f:
            SWITCH_TRACE(3);
            break;
        case SDLK_r:
            {
                Cell &cell = cells[highlight_uv];
                if (cell.device == NULL ) {
                    cell.device = &and_gate;
                } else if (cell.device == &and_gate) {
                    cell.device = &or_gate;
                } else if (cell.device == &or_gate) {
                    cell.device = NULL;
                }
            }
            break;
        }
    }
}

void LayerEntity::mouse_around(SDL_MouseMotionEvent &e) {
}
