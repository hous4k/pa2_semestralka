#include "Game.h"
#include "Entity.h"
#include <memory>
#include <fstream>

// -1 to initialize the first rendering driver supproting the requested flags
#define INDEX_OF_RENDERING_DRIVER -1
// not important 0 forn none
#define RENDERER_FLAG 0

SDL_Event Game::event;

//background color
#define RED 255
#define GREEN 255
#define BLUE 255
#define ALPHA 255


short UNIT = 32;

Game::Game ( const char * name,
             bool fullscreen  )
{
  // initialize SDL 0 return value mens no errors -1 means errors
  if ( SDL_Init ( SDL_INIT_EVERYTHING ) == 0 )
  {
    uint32_t flags = 0;
    if ( fullscreen )
    {
      flags = SDL_WINDOW_FULLSCREEN;
    }

    load_map ("level.map"); 

    win = SDL_CreateWindow ( name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, map_width, map_height, flags );

    renderer = SDL_CreateRenderer ( win, INDEX_OF_RENDERING_DRIVER, RENDERER_FLAG );


		if ( renderer )
		{
				SDL_SetRenderDrawColor ( renderer, RED, GREEN, BLUE, ALPHA );
				std::cout << "renderer created" << std::endl;
		}


    for ( auto & e : entities ){
      e -> set_renderer(renderer);
      e -> load_texture ();
    }


    is_running = true;
  }
  else
  {
    std::cout << "SDL_Error: " << SDL_GetError () << std::endl;
    is_running = false;
  }


}

void Game::update() {}

void Game::render()
{
	SDL_RenderClear ( renderer );
  for ( auto & e : entities )
  {
    e -> draw ();
  }
	SDL_RenderPresent ( renderer );
}

void Game::handle_events()
{
  // pops event from event queue
	while ( SDL_PollEvent ( &event ) )
  {
    if ( event . type == SDL_QUIT )
    {
      is_running = false;
    }
    else  
    {
      for ( auto & e : entities )
      {
        e -> on_event ( event );
      }
      /*
      if ( event . user . code == 0 )
        //std::cout << "timer" << std::endl;
      if ( event . user . code == 1 )
        //std::cout << "second timer" << std::endl;
      */

    }
  }
}

void Game::load_map ( const char * path )
{
    char c;
    std::fstream map_file;

    map_file . open ( path );

    size_t x_index = 0;
    size_t y_index = 0;

    while ( map_file . get ( c ) ) {
      std::cout << c;
      switch ( c ){
        case 'P':
          //add player entity
          entities . emplace_back ( std::make_unique<Player> ( Player ( x_index * UNIT,y_index * UNIT,UNIT,UNIT, "player.png" ) ) );
          ++x_index;
          break;

        case 'E':
          //add enemy entity
          entities . emplace_back ( std::make_unique<Enemy> ( Enemy ( x_index * UNIT,y_index * UNIT,UNIT,UNIT, "enemy.png" ) ) );
          ++x_index;
          break;

        case '#':
          //add wall entity
          entities . emplace_back ( std::make_unique<Wall> ( Wall ( x_index * UNIT,y_index * UNIT,UNIT,UNIT, "wall.png" ) ) );
          ++x_index;
          break;

        case '\n':
          map_width = x_index * UNIT;
          x_index = 0;
          ++y_index;
          break;

        default:
          //addbackground or nothing
          ++x_index;
          break;
      }
    }
    map_height = y_index * UNIT;

    map_file . close ();
}

Game::~Game()
{
	SDL_DestroyWindow ( win );
	SDL_DestroyRenderer ( Game::renderer );
	SDL_Quit();
  std::cout << "game cleaned" << std::endl;
}
