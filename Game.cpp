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
             bool fullscreen  ) : player ( 0,0,UNIT,UNIT, "player.png",0,0 )
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


    player . set_map_size ( map_width, map_height);
    player . set_renderer ( renderer );
    player . load_texture ();
    player . set_colliders ( &walls, &projectiles );

    for ( auto & e : enemies ){
      e -> set_renderer(renderer);
      e -> load_texture ();
    }

    for ( auto & w : walls ){
      w -> set_renderer(renderer);
      w -> load_texture ();
    }

    is_running = true;
  }
  else
  {
    std::cout << "SDL_Error: " << SDL_GetError () << std::endl;
    is_running = false;
  }


}

void Game::collisions() {
  // pro vsechny entity, ktery spolu muzou kolidovat koukni jestli nekolidujou
  // takze pro vsechny wally koukni jestli muzou kolidovat s playerem
  // pro vsechny enemies koukni jestli kolidujou s playerem
  // koukni jestli player nevysel z mapy
  // koukni jestli enemies nekolidujou s wallama nebo nevychazej z mapi
  // pro vsechny projektili koukni jestli nekoho nezasahli

  //for ( auto pr = std::begin (projectiles); pr != std::end(projectiles); ++pr )
  //{

  size_t pr_index = 0;
  for ( auto & pr : projectiles )
  {
    for( auto & w : walls ) {
      if ( pr )
      if ( pr -> intersects ( w.get() ))
      {
        pr . reset (nullptr);
        projectiles . erase (std::begin(projectiles)+pr_index);
        std::cout << "u hit a wall " << projectiles . size() << std::endl;
        
        break;
      }
      
    }
    continue;
    ++pr_index;
  }

//bonus
  pr_index = 0;
  for ( auto & pr : projectiles )
  {
    size_t enm_index = 0;
    for( auto & e : enemies ) {
      if (pr)
      if ( pr -> intersects ( e.get() ))
      {
        pr.reset(nullptr);
        projectiles . erase (std::begin(projectiles)+pr_index);
        //e . reset (nullptr);
        enemies . erase ( std::begin(enemies ) + enm_index );
        std::cout << "u hit an enemy" << std::endl;
        break;
      }
      ++enm_index;
    }
    ++pr_index;
  }

  for ( auto & e : enemies )
  {
    if ( e -> intersects ( &player ) )
    {
      quit ();
      is_running = false;
      std::cout << "Game Over!" << std::endl;
      break;
    }
  }

}

void Game::render()
{
	SDL_RenderClear ( renderer );
  for ( auto & e : enemies )
  {
    e -> draw ();
  }

  for ( auto & pr : projectiles )
  {
    if (pr)
    pr -> draw ();
  }

  for ( auto & w : walls )
  {
    w -> draw ();
  }

  player . draw ();

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
      player . on_event ( event );

      for ( auto & e : enemies )
      {
        e -> on_event ( event );
      }

      for ( auto & pr : projectiles )
      {
        if (pr)
        pr -> on_event ( event );
      }

      /*
      if ( event . user . code == 0 )
        //std::cout << "timer" << std::endl;
      if ( event . user . code == 1 )
        //std::cout << "second timer" << std::endl;
      */

    }

    if ( event.type == SDL_USEREVENT )
    {
      if ( event . user . code == 2 )
      {
        delete static_cast<int*>(event . user . data1);
        delete static_cast<int*>(event . user . data2);
      }
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

    bool player_flag = false;

    while ( map_file . get ( c ) ) {
      std::cout << c;
      switch ( c ){
        case 'P':
          //add player entity
          if ( ! player_flag ) {
            player . set_position ( x_index * UNIT, y_index * UNIT);
            player . set_map_size ( map_width, map_height );
            player_flag = true;
          }
          else
          {
            std::cout << "Error when loading map: \"multiple players has been set\"" << std::endl;
          }
          ++x_index;
          break;

        case 'E':
          //add enemy entity
          {
            std::unique_ptr<Enemy> tmp = std::make_unique<Enemy> ( Enemy ( x_index * UNIT,y_index * UNIT,UNIT,UNIT, "enemy.png" ) );
            //enemies . emplace_back ( std::make_unique<Enemy> ( Enemy ( x_index * UNIT,y_index * UNIT,UNIT,UNIT, "enemy.png" ) ) );
            tmp -> set_colliders ( &walls );
            enemies . emplace_back ( std::move(tmp));
            ++x_index;
          }
          break;

        case '#':
          //add wall entity
          walls . emplace_back ( std::make_unique<Wall> ( Wall ( x_index * UNIT,y_index * UNIT,UNIT,UNIT, "wall.png" ) ) );
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

    if ( ! player_flag ) { std::cout << "Player has not been set" << std::endl; }
    map_file . close ();
}

void Game::quit()
{
	SDL_DestroyWindow ( win );
	SDL_DestroyRenderer ( Game::renderer );
	SDL_Quit();
}

Game::~Game()
{
  quit();
  std::cout << "game cleaned" << std::endl;
}
