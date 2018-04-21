#include "entity.h"
#include "error.h"

// 0 is never a valid entity id.
entity_id_t unused_entity_id = 1;

Entity::Entity(Realm *r) : realm(r) {
    id = unused_entity_id++;
    if (realm != NULL) {
        realm->add_entity(this);
    }
}

Entity::~Entity(void) {}

void Entity::keyboard_input(SDL_KeyboardEvent &keyboard_event) {
    keyboard(keyboard_event);
}

#define FORWARD_MOUSE_INPUT(F, TYPE) \
    void Entity::mouse_##F##_input(TYPE &v) { \
        if (realm == NULL) { \
            mouse_##F(v); \
        } else { \
            if (realm->e_focused != 0) { \
                realm->get_entity(realm->e_focused)->mouse_##F(v); \
            } else { \
                mouse_##F(v); \
            } \
        } \
    }

FORWARD_MOUSE_INPUT(in, SDL_MouseMotionEvent)
FORWARD_MOUSE_INPUT(around, SDL_MouseMotionEvent)
FORWARD_MOUSE_INPUT(out, SDL_MouseMotionEvent)
FORWARD_MOUSE_INPUT(button, SDL_MouseButtonEvent)

Entity *Entity::get_entity(entity_id_t e) {
    if (e == 0) {
        ERROR("get_entity(0)");
    } else if (e == id) {
        return this;
    } else {
        return NULL;
    }
}

// Defaults for entities.. override these.
bool Entity::think(void) { return false; }
void Entity::draw(SDL_Renderer *, SDL_DisplayMode &) {}
void Entity::draw_input_pixels(SDL_Renderer *, SDL_DisplayMode &) {}
void Entity::keyboard(SDL_KeyboardEvent &) {}
void Entity::mouse_in(SDL_MouseMotionEvent &) {}
void Entity::mouse_around(SDL_MouseMotionEvent &) {}
void Entity::mouse_out(SDL_MouseMotionEvent &) {}
void Entity::mouse_button(SDL_MouseButtonEvent &) {}
