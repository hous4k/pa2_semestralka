#include "Game.h"
#include "Entity.h"
#include <memory>
#include <fstream>
#include <exception>

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


class multiple_player_defined : public std::exception
{
  virtual const char* what() const throw()
  {
    return "Multiple players has been set" ;
  }
};

class wrong_map_format : public std::exception
{
  virtual const char* what() const throw()
  {
    return "Wrong map format" ;
  }
};

class unset_player : public std::exception
{
  virtual const char* what() const throw()
  {
    return "The player has not been set" ;
  }
};

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

bool coin_flip ( size_t prob ) {
  std::random_device r;
  std::mt19937 random_engine (r());
  std::uniform_int_distribution<int> uniform_dist(1,100);
  if ( uniform_dist ( random_engine ) <= prob )
  {
    return true;
  }
  return false;
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

//bonus TODO
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
        if( coin_flip ( 100 ) ) {
          Vector2 v = enemies [ enm_index ] -> get_position();
          std::cout << "bonus granted" << std::endl;
          std::unique_ptr<Bonus> tmp =std::make_unique<Bonus> ( Bonus ( v . x , v . y ,UNIT,UNIT, "bonus.png" ) ); 
          tmp -> set_renderer(renderer);
          tmp -> load_texture ();
          bonuses . emplace_back (  std::move( tmp ) );
          std::cout << bonuses . size() << std::endl;
        }
        enemies . erase ( std::begin(enemies ) + enm_index );
        // s urcitou pravdepodobnosti poloz bonus

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

  size_t b_index = 0;
  for ( auto & b : bonuses )
  {
    if ( b );
    if ( b -> intersects ( &player ) )
    { 
      b . reset ( nullptr );
      bonuses . erase ( std::begin( bonuses ) + b_index );
      if ( coin_flip ( 80 ) )
      {
        player . up_reload_time();
        std::cout << "reload time bonus" << std::endl;
      }
      else
      {
        player . up_speed();
        std::cout << "speed bonus" << std::endl;
      }
      break;
    }
    ++b_index;
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

  for ( auto & b : bonuses )
  {
    b -> draw ();
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
      /*
      if ( event . user . code == 5 )
      {
        delete  static_cast<Entity*>(event . user . data1 );
        std::cout << "has been destoryed" << std::endl;
      }
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

    bool player_flag = false;

    int prev_width = -1;

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
            throw multiple_player_defined();
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
          if ( prev_width > -1 ) 
          {
            if ( prev_width != x_index )
            {
              throw wrong_map_format();
            }
          }
          prev_width = x_index;
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

    if ( ! player_flag ) 
    { 
      throw unset_player();
    }
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
