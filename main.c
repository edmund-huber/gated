#include <SDL.h>
#include <SDL_image.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"

typedef struct {
    int n_inputs;
    bool labeled_inputs;
    int n_outputs;
    void (*fn)(int *, int *);
} device_t;

#define BASIC_DEVICE(NAME, EXPR) \
    void NAME##_device_fn(int *inputs, int *outputs) { \
        int a = inputs[0], b = inputs[1]; \
        outputs[0] = EXPR; \
    } \
    \
    device_t NAME##_device = { \
        .n_inputs = 2, \
        .labeled_inputs = false, \
        .n_outputs = 1, \
        .fn = NAME##_device_fn \
    };

BASIC_DEVICE(and, a & b);

typedef struct {
    device_t *device;
    int traces[4];
} cell_t;

#define CELLS_X 16
#define CELLS_Y 16
cell_t cells[CELLS_X][CELLS_Y];

void print_display_mode(int i, SDL_DisplayMode *mode) {
    if (i == -1) {
        printf("    -> ");
    } else {
        printf("    %i: ", i);
    }
    printf("%ix%ix%i %s %ihz\n",
        mode->w, mode->h, SDL_BITSPERPIXEL(mode->format),
        SDL_GetPixelFormatName(mode->format), mode->refresh_rate);
}

SDL_Renderer *rend;
SDL_Surface *trace_surface;
SDL_Texture *tex;

void render() {
    int cell_w = 32;
    int cell_h = 32;
    for (int i = 0; i < CELLS_X; i++) {
        for (int j = 0; j < CELLS_Y; j++) {
            SDL_Rect rect = {
                .x = cell_w * i,
                .y = cell_h * j,
                .w = cell_w,
                .h = cell_h
            };
            SDL_Rect src = { .x = 0, .y = 0, .w = 64, .h = 64 };
            if (i % 2 == 0) {
                SDL_SetTextureColorMod(tex, 0, 0xff, 0);
            } else {
                SDL_SetTextureColorMod(tex, 0, 0, 0xff);
            }
            cell_t *cell = &cells[i][j];
            if (cell->traces[0]) {
                SDL_RenderCopyEx(rend, tex, &src, &rect, 0, NULL, SDL_FLIP_NONE);
            }
            if (cell->traces[1]) {
                SDL_RenderCopyEx(rend, tex, &src, &rect, 90, NULL, SDL_FLIP_NONE);
            }
            if (cell->traces[2]) {
                SDL_RenderCopyEx(rend, tex, &src, &rect, 180, NULL, SDL_FLIP_NONE);
            }
            if (cell->traces[3]) {
                SDL_RenderCopyEx(rend, tex, &src, &rect, 270, NULL, SDL_FLIP_NONE);
            }
        }
    }
}

int main() {
    puts("version " VERSION);

    ASSERT_SDL(SDL_Init(SDL_INIT_VIDEO) == 0);
    int flags = IMG_INIT_PNG;
    ASSERT((IMG_Init(flags) & flags) == flags);

    // List all displays and their modes.
    int num_displays = CHECK_SDL_RET(SDL_GetNumVideoDisplays());
    printf("number of displays: %i\n", num_displays);
    for (int i = 0; i < num_displays; i++) {
        printf("display %i:\n", i);
        SDL_DisplayMode mode;
        for (int j = 0; j < CHECK_SDL_RET(SDL_GetNumDisplayModes(i)); j++) {
            CHECK_SDL_RET(SDL_GetDisplayMode(i, j, &mode));
            print_display_mode(j, &mode);
        }
        // Point out the current display mode.
        CHECK_SDL_RET(SDL_GetCurrentDisplayMode(i, &mode));
        print_display_mode(-1, &mode);
    }

    // Make a fullscreen window with OpenGL capability on display 0.
    SDL_DisplayMode mode;
    CHECK_SDL_RET(SDL_GetCurrentDisplayMode(0, &mode));
    SDL_Window *window = SDL_CreateWindow(
        "gated",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        mode.w,
        mode.h,
        0
    );
    ASSERT_SDL(window != NULL);

    rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);
    ASSERT_SDL(rend != NULL);

    for (int i = 0; i < CELLS_X; i++) {
        for (int j = 0; j < CELLS_Y; j++) {
            cell_t *cell = &cells[i][j];
            cell->device = NULL;
            cell->traces[0] = 0;
            cell->traces[1] = 0;
            cell->traces[2] = 0;
            cell->traces[3] = 0;
        }
    }

    trace_surface = IMG_Load("trace.png");
    tex = SDL_CreateTextureFromSurface(rend, trace_surface);

    SDL_Texture *tex_click = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGB888,
        SDL_TEXTUREACCESS_TARGET, mode.w, mode.h);
    ASSERT_SDL(tex_click != NULL);

    int quit = 0;
    bool show_click_map = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_q:
                    quit = 1;
                    break;
                case SDLK_0:
                    show_click_map = !show_click_map;
                    break;
                }
            }
        }

        CHECK_SDL_RET(SDL_SetRenderTarget(rend, tex_click));
        CHECK_SDL_RET(SDL_SetRenderDrawColor(rend, 0, 0xff, 0, 0));
        CHECK_SDL_RET(SDL_RenderClear(rend));

        CHECK_SDL_RET(SDL_SetRenderTarget(rend, NULL));
        SDL_SetRenderDrawColor(rend, 0xff, 0, 0xff, 0xff);
        SDL_RenderClear(rend);
        render();
        if (show_click_map) {
            CHECK_SDL_RET(SDL_RenderCopy(rend, tex_click, NULL, NULL));
        }
        SDL_RenderPresent(rend);

        // Not a busy loop.
        SDL_Delay(10);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
