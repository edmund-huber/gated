#include <SDL.h>
#include <SDL_image.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"

SDL_Surface *trace_surface;
SDL_Texture *tex;

int cell_w = 32;
int cell_h = 32;

void loop(SDL_Renderer *);

int main(void) {
    puts("version " VERSION);

    ASSERT_SDL(SDL_Init(SDL_INIT_VIDEO) == 0);
    int flags = IMG_INIT_PNG;
    ASSERT((IMG_Init(flags) & flags) == flags);

    // Make a fullscreen window on display 0.
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
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);
    ASSERT_SDL(renderer != NULL);

    loop(renderer);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
