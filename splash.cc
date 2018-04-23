#include <SDL.h>

#include "error.h"
#include "entity.h"

SplashEntity::SplashEntity(Realm *r) : Entity(r) {
    done = false;
    time_begin = realm->time_in_seconds();
    time_end = time_begin + 2;
    moused = false;
    clicked_when = 0;
}

void SplashEntity::draw(SDL_Renderer *renderer, SDL_DisplayMode &mode) {
    if (realm->time_in_seconds() - clicked_when < 1) {
        CHECK_SDL_RET(SDL_SetRenderDrawColor(renderer, 0, 0, 0xff, 0xff));
    } else if (moused) {
        CHECK_SDL_RET(SDL_SetRenderDrawColor(renderer, 0xff, 0, 0, 0xff));
    } else {
        CHECK_SDL_RET(SDL_SetRenderDrawColor(renderer, 0xaa, 0, 0, 0xff));
    }
    draw_input_pixels(renderer, mode);
}

void SplashEntity::draw_input_pixels(SDL_Renderer *renderer, SDL_DisplayMode &mode) {
    SDL_Rect rect = {
        .x = (mode.w / 2) - 50,
        .y = (mode.h / 2) - 50,
        .w = 100,
        .h = 100
    };
    CHECK_SDL_RET(SDL_RenderFillRect(renderer, &rect));
}

bool SplashEntity::think(void) {
    if (done) {
        new UIEntity(realm);
        return true;
    }
    return false;
}

void SplashEntity::keyboard(SDL_KeyboardEvent &keyboard_event) {
    if (realm->time_in_seconds() > time_end) {
        if (keyboard_event.type == SDL_KEYDOWN) {
            done = true;
        }
    }
}

void SplashEntity::mouse_in(SDL_MouseMotionEvent &) {
    moused = true;
}

void SplashEntity::mouse_out(SDL_MouseMotionEvent &) {
    moused = false;
}

void SplashEntity::mouse_button(SDL_MouseButtonEvent &) {
    clicked_when = realm->time_in_seconds();
}
