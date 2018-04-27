#include <SDL.h>

#include "error.h"
#include "entity.h"

UIEntity::UIEntity(Realm *r) : Entity(r) {
    done = false;
    new LayerEntity(realm);
}

void UIEntity::draw(SDL_Renderer *renderer, SDL_DisplayMode &mode) {
    /*CHECK_SDL_RET(SDL_SetRenderDrawColor(renderer, 0, 0xff, 0, 0xff));
    SDL_Rect rect = {
        .x = (mode.w / 2) - 50,
        .y = (mode.h / 2) - 50,
        .w = 100,
        .h = 100
    };
    CHECK_SDL_RET(SDL_RenderFillRect(renderer, &rect));*/
}

bool UIEntity::think(void) {
    if (done) {
        realm->finished = true;
        return true;
    }
    return false;
}

void UIEntity::keyboard(SDL_KeyboardEvent &keyboard_event) {
    switch (keyboard_event.keysym.sym) {
    case SDLK_q:
        done = true;
    }
}
