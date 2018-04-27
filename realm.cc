#include <time.h>

#include "entity.h"
#include "error.h"

int Realm::input_pixel_shift = 0;

Realm::Realm(void) : Entity(NULL) {
    // A standalone realm.
    dummy_entity = new DummyEntity(this);
    e_focused = 0;
    finished = false;
}

Realm::Realm(Realm *r) : Entity(r) {
    // An embedded realm.
    dummy_entity = new DummyEntity(this);
    e_focused = 0;
    finished = false;
}

void Realm::add_entity(Entity *p) {
    entities.push_back(p);
}

Entity *Realm::get_entity(entity_id_t e) {
    ASSERT(e != 0);
    if (e == id) {
        return this;
    }
    for (list<Entity *>::iterator i = entities.begin(); i != entities.end(); i++) {
        Entity *entity = (*i)->get_entity(e);
        if (entity != NULL) {
            return entity;
        }
    }
    return dummy_entity;
}

bool Realm::think(void) {
    for (list<Entity *>::iterator i = entities.begin(); i != entities.end(); ) {
        if ((*i)->think()) {
            // If requested, delete the entity (and remove from the realm).
            delete *i;
            entities.erase(i++);
        } else {
            i++;
        }
    }
    return finished;
}

void Realm::draw(SDL_Renderer *renderer, SDL_DisplayMode &mode) {
    for (list<Entity *>::iterator i = entities.begin(); i != entities.end(); i++) {
        (*i)->draw(renderer, mode);
    }
}

void Realm::draw_input_pixels(SDL_Renderer *renderer, SDL_DisplayMode &mode) {
    for (list<Entity *>::iterator i = entities.begin(); i != entities.end(); i++) {
        Entity *e = *i;
        ASSERT(sizeof(e->id) == 8);
        uint64_t v = e->id >> Realm::input_pixel_shift;
        CHECK_SDL_RET(SDL_SetRenderDrawColor(renderer, v & 0xf000, v & 0x0f00, v & 0x00f0, v & 0x000f));
        e->draw_input_pixels(renderer, mode);
    }
}

void Realm::keyboard(SDL_KeyboardEvent &ev) {
    // See if something in the keymap is set up to intercept this
    // keypress.
    if (ev.type == SDL_KEYDOWN) {
        if (keymap.end() != keymap.find(ev.keysym.sym)) {
            Entity *entity = keymap[ev.keysym.sym];
            entity->keyboard_input(ev);
            return;
        }
    }

    // The focused entity gets the input if there is one, otherwise
    // every entity gets the input.
    if (e_focused != 0) {
        get_entity(e_focused)->keyboard_input(ev);
    } else {
        for (list<Entity *>::iterator i = entities.begin(); i != entities.end(); i++) {
            (*i)->keyboard_input(ev);
        }
    }
}

// The realm paints its background so that it can forward mouse events
// to the focused entity.
#define FORWARD_MOUSE_INPUT(F, TYPE) \
    void Realm::mouse_##F(TYPE &v) { \
        if (e_focused != 0) { \
            get_entity(e_focused)->mouse_##F##_input(v); \
        } \
    }

FORWARD_MOUSE_INPUT(in, SDL_MouseMotionEvent)
FORWARD_MOUSE_INPUT(around, SDL_MouseMotionEvent)
FORWARD_MOUSE_INPUT(out, SDL_MouseMotionEvent)
FORWARD_MOUSE_INPUT(button, SDL_MouseButtonEvent)

double Realm::time_in_seconds(void) {
    // Check for reasonable resolution in the monotonic clock.
    timespec ts;
    ASSERT(clock_getres(CLOCK_MONOTONIC, &ts) == 0);
    ASSERT((ts.tv_sec == 0) && (ts.tv_nsec <= 1));

    // Get the time.
    ASSERT(clock_gettime(CLOCK_MONOTONIC, &ts) == 0);
    return ts.tv_sec + (ts.tv_nsec / (double)1e9);
}
