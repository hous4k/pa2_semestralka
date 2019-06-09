#include "Game.h"
#include "Entity.h"
#include <memory>
#include <fstream>
#include <exception>

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
             bool fullscreen  ) : player ( 0,0,UNIT,UNIT, "../data/player.png",0,0 )
{
  // initialize SDL 0 return value mens no errors -1 means errors
  if ( SDL_Init ( SDL_INIT_EVERYTHING ) == 0 )
  {
    uint32_t flags = 0;
    if ( fullscreen )
    {
      flags = SDL_WINDOW_FULLSCREEN;
    }

    load_map ("../data/level.map"); 

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

    banner . init ( (map_width - PREPARE_WIDTH )/2 , (map_height - PREPARE_HEIGHT) / 2, PREPARE_WIDTH, PREPARE_HEIGHT, "../data/prepare.png"  );
    banner . set_renderer ( renderer );
    banner . load_texture ();

    for ( auto & e : enemies ){
      e -> set_renderer(renderer);
      e -> load_texture ();
    }

    for ( auto & w : walls ){
      w -> set_renderer(renderer);
      w -> load_texture ();
    }

    is_running = true;
    banner_render( banner );
    banner . delay (PREPARE_DELAY);
  }
  else
  {
    std::cout << "SDL_Error: " << SDL_GetError () << std::endl;
    is_running = false;
  }


}

bool coin_flip ( unsigned short prob ) {
  std::random_device r;
  std::mt19937 random_engine (r());
  std::uniform_int_distribution<int> uniform_dist(1,100);
  if ( static_cast<unsigned short> ( uniform_dist ( random_engine ) ) <= prob )
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

  for ( auto p_it = projectiles . begin(); p_it != projectiles . end(); ) {
    bool deleted = false;
      for ( auto & w : walls )
      {
        if ( (*p_it) -> intersects ( w . get () ))
        {
          p_it = projectiles . erase ( p_it );
          std::cout << "u hit a wall " << projectiles . size() << std::endl;
          deleted = true;
          break;
        }
      }
      if ( ! deleted )
      {
        ++p_it;
      }
  }

  for ( auto p_it = projectiles . begin(); p_it != projectiles . end(); ) {
    Vector2 position = (*p_it) -> get_position ();  
    if ( position . x < 0 || position . y < 0 || position . x > map_width || position . y > map_height )
    {
      p_it = projectiles . erase ( p_it );
      std::cout << "u shoot map borders " << projectiles . size() << std::endl;
      break;
    }
    else 
    {
      ++p_it;
    }
  }


  for ( auto p_it = projectiles . begin (); p_it != projectiles . end(); ) {
    bool deleted = false;
      for ( auto e_it = enemies . begin (); e_it != enemies . end (); )
      {
        if ( (*p_it) -> intersects ( e_it -> get () ))
        {
          p_it = projectiles . erase ( p_it );
          if( coin_flip ( 100 ) ) {
            Vector2 v = ( *e_it ) -> get_position();
            std::cout << "bonus granted" << std::endl;
            std::unique_ptr<Bonus> tmp =std::make_unique<Bonus> ( v . x , v . y , UNIT,UNIT, "../data/bonus.png" ); 
            tmp -> set_renderer(renderer);
            tmp -> load_texture ();
            bonuses . emplace_back (  std::move( tmp ) );
          }
          e_it = enemies . erase ( e_it );
          deleted = true;
          break;
        }
        else
        {
          ++e_it; 
        }
      }
      if ( ! deleted )
      {
        ++p_it;
      }

      if ( enemies . size () == 0 )
      {
        banner . init ( (map_width - WIN_WIDTH )/2 , (map_height - WIN_HEIGHT) / 2, WIN_WIDTH, WIN_HEIGHT, "../data/win.png"  );
        banner . set_renderer ( renderer );
        banner . load_texture ();
        banner_render ( banner );
        banner . delay ( WIN_DELAY );
        is_running = false;
      }
  }

  for ( auto & e : enemies )
  {
    if ( e -> intersects ( &player ) )
    {
      std::cout << "Game Over!" << std::endl;
      banner . init ( (map_width - GAME_OVER_WIDTH )/2 , (map_height - GAME_OVER_HEIGHT) / 2, GAME_OVER_WIDTH, GAME_OVER_HEIGHT, "../data/game_over.png"  );
      banner . set_renderer ( renderer );
      banner . load_texture ();
      banner_render ( banner );
      banner . delay ( GAME_OVER_DELAY );

      is_running = false;
      break;
    }
  }

  for ( auto b_it = bonuses . begin(); b_it != bonuses . end(); ) {
      if ( player . intersects ( b_it -> get () ))
      {
        b_it = bonuses . erase ( b_it );
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
      else
      {
        ++b_it;
      }
  }

}

void Game::banner_render ( Banner & ban )
{
	SDL_RenderClear ( renderer );
  for ( auto & e : enemies )
  {
    e -> draw ();
  }

  for ( auto & pr : projectiles )
  {
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

  ban . draw ();

	SDL_RenderPresent ( renderer );

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
        pr -> on_event ( event );
      }

      for ( auto & w : walls )
      {
        w -> on_event ( event );
      }

      for ( auto & b : bonuses )
      {
        b -> on_event ( event );
      }

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
            std::unique_ptr<Enemy> tmp = std::make_unique<Enemy> ( x_index * UNIT,y_index * UNIT,UNIT,UNIT, "../data/enemy.png" );
            tmp -> set_colliders ( &walls );
            enemies . emplace_back ( std::move(tmp));
            ++x_index;
          }
          break;

        case '#':
          //add wall entity
          walls . emplace_back ( std::make_unique<Wall> ( x_index * UNIT,y_index * UNIT,UNIT,UNIT, "../data/wall.png" ) );
          ++x_index;
          break;

        case '\n':
          if ( prev_width > -1 ) 
          {
            if (  prev_width != static_cast<int> ( x_index ) )
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
