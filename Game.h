#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "Entity.h"
#include <iostream>
#include <vector>
#include <memory>
#include <random>

extern Uint32 MY_EVENT_TYPE; 

class Game {
  
  public:
    //const char * required by sdl functions
    Game ( const char * name,
           bool fullscreen );
    ~Game ();

    bool running () { return is_running; }

    void handle_events ();
    void collisions ();
    void render ();
    void load_map ( const char * path );
    void quit();

    static SDL_Event event;
  private:
    // variable wher window will be rendered to
    size_t map_width;
    size_t map_height;
    SDL_Window * win;
    bool is_running;
    SDL_Renderer * renderer;

    std::vector<std::unique_ptr<Entity>> enemies;

    std::vector<std::unique_ptr<Entity>> walls;
    std::vector<std::unique_ptr<Entity>> projectiles;
    std::vector<std::unique_ptr<Entity>> bonuses;
    Player player;

};
