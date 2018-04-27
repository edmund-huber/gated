#include <SDL.h>

#include "error.h"
#include "entity.h"

LayerEntity::LayerEntity(Realm *r) : Entity(r) {
    offset_x = 0;
    offset_y = 0;
    zoom = 1;
}

void LayerEntity::draw(SDL_Renderer *renderer, SDL_DisplayMode &mode) {
    CHECK_SDL_RET(SDL_SetRenderDrawColor(renderer, 0xff, 0, 0, 0xff));

    int32_t size = zoom * 16, margin = zoom;
    int32_t min_x = ((offset_x - size) / size) * size;
    for (int32_t x = min_x; x - offset_x < mode.w; x += size) {
        int min_y = ((offset_y - size) / size) * size;
        for (int32_t y = min_y; y - offset_y < mode.h; y += size) {
            SDL_Rect rect = {
                .x = x - offset_x,
                .y = y - offset_y,
                .w = size - (margin * 2),
                .h = size - (margin * 2)
            };
            CHECK_SDL_RET(SDL_RenderFillRect(renderer, &rect));
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
        switch (keyboard_event.keysym.sym) {
        case SDLK_z:
            zoom += 1;
            break;
        case SDLK_x:
            if (zoom > 1) {
                zoom -= 1;
            }
            break;
        }
    }
}

void LayerEntity::mouse_around(SDL_MouseMotionEvent &e) {
    offset_x += e.xrel;
    offset_y += e.yrel;
}
