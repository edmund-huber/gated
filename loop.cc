#include <inttypes.h>
#include <SDL.h>
#include <time.h>

#include "entity.h"
#include "error.h"

static SDL_DisplayMode display_mode;
static SDL_Texture *input_texture_1 = NULL;
static SDL_Texture *input_texture_2 = NULL;

static void recreate_input_texture(SDL_Renderer *renderer, SDL_Texture **texture) {
    // Create a texture that can be used as a render target.
    if (*texture != NULL) {
        SDL_DestroyTexture(*texture);
    }
    *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        display_mode.w,
        display_mode.h
    );
    ASSERT_SDL(*texture != NULL);
    // Blank it.
    CHECK_SDL_RET(SDL_SetRenderTarget(renderer, *texture));
    CHECK_SDL_RET(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0));
    CHECK_SDL_RET(SDL_RenderClear(renderer));
}

static void change_display_mode(SDL_Renderer *renderer) {
    // Get the new display mode details.
    CHECK_SDL_RET(SDL_GetCurrentDisplayMode(0, &display_mode));
    printf("display: w=%i, h=%i\n", display_mode.w, display_mode.h);

    // (Re?)create the input textures.
    recreate_input_texture(renderer, &input_texture_1);
    recreate_input_texture(renderer, &input_texture_2);
}

void loop(SDL_Renderer *renderer) {
    // The set of starting entities.
    Realm *r = new Realm();
    new SplashEntity(r);

    time_t then = 0;
    int frames_since_then;
    entity_id_t e_mouse_previous = 0, e_mouse_current = 0;
    change_display_mode(renderer);
    while (true) {
        // Calculate FPS.
        time_t now = time(NULL);
        if (now > then) {
            printf("%ifps\n", frames_since_then);
            frames_since_then = 0;
            then = now;
        }
        frames_since_then++;

        // Events!
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {

            // Pass all keyboard inputs to the realm.
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                r->keyboard_input(ev.key);
                break;

            case SDL_MOUSEMOTION:
                // Figure out what entity is being moused.
                {
                    CHECK_SDL_RET(SDL_SetRenderTarget(renderer, input_texture_1));
                    SDL_Rect rect = { .x = ev.motion.x, .y = ev.motion.y, .w = 1, .h = 1 };
                    uint64_t pixel_1;
                    CHECK_SDL_RET(SDL_RenderReadPixels(
                        renderer,
                        &rect,
                        SDL_PIXELFORMAT_RGBA8888,
                        &pixel_1,
                        display_mode.w * 4
                    ));
                    CHECK_SDL_RET(SDL_SetRenderTarget(renderer, input_texture_2));
                    uint64_t pixel_2;
                    CHECK_SDL_RET(SDL_RenderReadPixels(
                        renderer,
                        &rect,
                        SDL_PIXELFORMAT_RGBA8888,
                        &pixel_2,
                        display_mode.w * 4
                    ));
                    ASSERT(sizeof(e_mouse_current) == 8);
                    e_mouse_current = (pixel_2 << 32) | pixel_1;
                }

                if (e_mouse_current == e_mouse_previous) {
                    if (e_mouse_previous != 0) {
                        MSG_WITH_ARGS("mouse_around on %" PRIx64, e_mouse_previous);
                        r->get_entity(e_mouse_previous)->mouse_around_input(ev.motion);
                    }
                } else {
                    if (e_mouse_previous != 0) {
                        MSG_WITH_ARGS("mouse_out on %" PRIx64, e_mouse_previous);
                        r->get_entity(e_mouse_previous)->mouse_out_input(ev.motion);
                    }
                    if (e_mouse_current != 0) {
                        MSG_WITH_ARGS("mouse_in on %" PRIx64, e_mouse_current);
                        r->get_entity(e_mouse_current)->mouse_in_input(ev.motion);
                    }
                }
                e_mouse_previous = e_mouse_current;
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                // Ignoring x and y, because we should have gotten an
                // SDL_MouseMotionEvent for that. I'm not sure that's safe.
                if (e_mouse_current != 0) {
                    MSG_WITH_ARGS("mouse_button on %" PRIx64, e_mouse_current);
                    r->get_entity(e_mouse_current)->mouse_button_input(ev.button);
                }
                break;

            case SDL_WINDOWEVENT:
                switch (ev.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    change_display_mode(renderer);
                    break;
                }
                break;
            }
        }

        // All entities get to think now.
        if (r->think()) {
            break;
        }

        // Set up the input regions: we split the entity_id_t's across two
        // textures.
        CHECK_SDL_RET(SDL_SetRenderTarget(renderer, input_texture_1));
        CHECK_SDL_RET(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0));
        CHECK_SDL_RET(SDL_RenderClear(renderer));
        Realm::input_pixel_shift = 0;
        r->draw_input_pixels(renderer, display_mode);
        CHECK_SDL_RET(SDL_SetRenderTarget(renderer, input_texture_2));
        CHECK_SDL_RET(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0));
        CHECK_SDL_RET(SDL_RenderClear(renderer));
        Realm::input_pixel_shift = 32;
        r->draw_input_pixels(renderer, display_mode);

        // Then draw everything and flip buffers.
        CHECK_SDL_RET(SDL_SetRenderTarget(renderer, NULL));
        CHECK_SDL_RET(SDL_SetRenderDrawColor(renderer, 0xff, 0, 0xff, 0xff));
        CHECK_SDL_RET(SDL_RenderClear(renderer));
        r->draw(renderer, display_mode);
        SDL_RenderPresent(renderer);
    }
}
