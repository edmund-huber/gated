#include <SDL.h>
#include <SDL_image.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"

typedef struct {
} gate_t;

typedef struct {
    gate_t gate;
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

    SDL_Renderer *rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);
    ASSERT_SDL(rend != NULL);
    SDL_SetRenderDrawColor(rend, 0xff, 0, 0xff, 0xff);
    SDL_RenderClear(rend);

    for (int i = 0; i < CELLS_X; i++) {
        for (int j = 0; j < CELLS_Y; j++) {
            cell_t *cell = &cells[i][j];
            cell->traces[0] = random() % 2;
            cell->traces[1] = random() % 2;
            cell->traces[2] = random() % 2;
            cell->traces[3] = random() % 2;
        }
    }

    SDL_Surface *trace_surface = IMG_Load("trace.png");
    SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, trace_surface);

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
    SDL_RenderPresent(rend);

    int quit = 0;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_q:
                    quit = 1;
                    break;
                }
            }
            break;
        }
        // Not a busy loop.
        SDL_Delay(10);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
