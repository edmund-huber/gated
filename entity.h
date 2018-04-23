#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <list>
#include <map>
#include <SDL.h>
using namespace std;

typedef uint64_t entity_id_t;
typedef uint64_t tick_t;

class ClientEntity;
class Realm;
class Entity {
public:
    Entity(Realm *);
    virtual ~Entity(void);

    virtual bool think(void);
    virtual void draw(SDL_Renderer *, SDL_DisplayMode &);
    virtual void draw_input_pixels(SDL_Renderer *, SDL_DisplayMode &);

    void keyboard_input(SDL_KeyboardEvent &);
    void mouse_in_input(SDL_MouseMotionEvent &);
    void mouse_around_input(SDL_MouseMotionEvent &);
    void mouse_out_input(SDL_MouseMotionEvent &);
    void mouse_button_input(SDL_MouseButtonEvent &);

    // For entities that contain other entities, this will search
    // within.
    virtual Entity *get_entity(entity_id_t);

    // The realm that this entity is a part of, and its id in the realm.
    Realm * const realm;
    entity_id_t id;

private:
    // Input methods -- override these.
    virtual void keyboard(SDL_KeyboardEvent &);
    virtual void mouse_in(SDL_MouseMotionEvent &);
    virtual void mouse_around(SDL_MouseMotionEvent &);
    virtual void mouse_out(SDL_MouseMotionEvent &);
    virtual void mouse_button(SDL_MouseButtonEvent &);
};

// Entity definitions,

class Realm : public Entity {
public:
    Realm(void);
    Realm(Realm *);

    void add_entity(Entity *);
    Entity *get_entity(entity_id_t);

    // If there is a focused entity, only it gets the input.
    entity_id_t e_focused;

    double time_in_seconds(void);

    bool think(void);
    void draw(SDL_Renderer *, SDL_DisplayMode &);
    void draw_input_pixels(SDL_Renderer *, SDL_DisplayMode &);

    // For keyboard focus for certain keys (e.g., key to get to the
    // menus).
    map<SDL_Keycode, Entity *> keymap;

    bool finished;

    static int input_pixel_shift;

private:
    list<Entity *> entities;

    // Forwarding input events to the focused entity.
    void keyboard(SDL_KeyboardEvent &);
    void mouse_in(SDL_MouseMotionEvent &);
    void mouse_around(SDL_MouseMotionEvent &);
    void mouse_out(SDL_MouseMotionEvent &);
    void mouse_button(SDL_MouseButtonEvent &);
};

class SplashEntity : public Entity {
public:
    SplashEntity(Realm *);
    bool think(void);
    void draw(SDL_Renderer *, SDL_DisplayMode &);
    void draw_input_pixels(SDL_Renderer *, SDL_DisplayMode &);
    void keyboard(SDL_KeyboardEvent &);

private:
    bool done;
    double time_begin;
    double time_end;
    bool moused;
    double clicked_when;

    void mouse_in(SDL_MouseMotionEvent &);
    void mouse_out(SDL_MouseMotionEvent &);
    void mouse_button(SDL_MouseButtonEvent &);
};

class UIEntity : public Entity {
public:
    UIEntity(Realm *);
    bool think(void);
    void draw(SDL_Renderer *, SDL_DisplayMode &);
    void keyboard(SDL_KeyboardEvent &);

private:
    bool done;
};

#endif
