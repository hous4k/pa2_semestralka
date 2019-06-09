#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "Entity.h"
#include <iostream>
#include <vector>
#include <memory>
#include <random>

// create_renderer paramaters
#define INDEX_OF_RENDERING_DRIVER -1
#define RENDERER_FLAG 0

// background color
#define RED 255
#define GREEN 255
#define BLUE 255
#define ALPHA 255

// prepare banner size
#define PREPARE_WIDTH 512
#define PREPARE_HEIGHT 64
#define PREPARE_DELAY 3000

// game over banner size
#define GAME_OVER_WIDTH 512
#define GAME_OVER_HEIGHT 128
#define GAME_OVER_DELAY 3000

// win banner size
#define WIN_WIDTH 384
#define WIN_HEIGHT 128
#define WIN_DELAY 3000

// one unit size similar to metter
#define UNIT 32

class Game {
  
  public:
    //const char * required by sdl functions
    Game ( const char * name,
           bool fullscreen );
    ~Game ();

    bool running () { return is_running; }

    void handle_events ();
    void collisions ();
    void banner_render ( Banner & ban );
    void render ();
    void load_map ( const char * path );
    void quit();

    SDL_Event event;
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

    Banner banner;
};
