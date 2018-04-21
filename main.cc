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
