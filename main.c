#include <SDL.h>
#include <signal.h>

#define xstr(s) str(s)
#define str(s) #s

#define MSG(s) \
    puts(__FILE__ ", L" xstr(__LINE__) ": " s "\n");

#define ERROR(s) \
    MSG(s); \
    raise(3); \
    exit(1);

#define ASSERT(c) \
    if (!(c)) { \
        ERROR("failed: ASSERT(" xstr(c) ")") \
    }

int main() {
    ASSERT(SDL_Init(SDL_INIT_VIDEO) == 0);
    SDL_Window *window = SDL_CreateWindow(
        "gated",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        800,
        600,
        SDL_WINDOW_OPENGL
    );
    puts(SDL_GetError());
    ASSERT(window != NULL);

    SDL_Delay(5000);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
