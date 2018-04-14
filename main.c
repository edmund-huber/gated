#include <SDL.h>
#include <signal.h>
#include <stdio.h>

#include "error.h"

typedef struct {
} gate_t;

typedef struct {
} margin_t;

typedef struct {
    gate_t gate;
    margin_t margins[4];
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
    ASSERT_SDL(SDL_Init(SDL_INIT_VIDEO) == 0);

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
            int red = i % 2 == 0 ? 0xff : 0;
            int blue = (j + 1) % 2 == 0 ? 0xff : 0;
            SDL_SetRenderDrawColor(rend, red, 0, blue, 0xff);
            SDL_RenderFillRect(rend, &rect);
        }
    }

    SDL_RenderPresent(rend);

    SDL_Delay(5000);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
