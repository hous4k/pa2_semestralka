#include "Entity.h"

Entity::Entity (){}

Entity::Entity ( int x, int y, int w, int h, const char * path ) 
  : position ( x , y  )
  , velocity ( 0 , 0 )
  , width ( w )
  , height ( h )
  , angle ( 0.0 )
  , flip ( SDL_FLIP_NONE )
  , renderer(NULL)
{
  src_rect . x = 0;
  src_rect . y = 0;
  src_rect . w = w;
  src_rect . h = h;

  dst_rect . x = x;
  dst_rect . y = y;
  dst_rect . w = w;
  dst_rect . h = h;

  texture_path = path;
}

Vector2 Entity::get_position ()
{
  return position;
}

Vector2 Entity::get_width_height ()
{
  return Vector2 ( width, height );
}

void Entity::set_position ( int x, int y )
{
  position . x = x;
  position . y = y;

  dst_rect . x = x;
  dst_rect . y = y;
}

bool Entity::intersects ( Entity * e )
{
  if (
    dst_rect . x + dst_rect . w  >= e -> dst_rect . x &&
    dst_rect . y + dst_rect . h  >= e -> dst_rect . y &&
    e -> dst_rect . x + e -> dst_rect . w >= dst_rect . x &&
    e -> dst_rect . y + e -> dst_rect . h >= dst_rect . y
  )
  {
    return true;
  }
  return false;
}

bool Entity::intersects ( int x, int y, int height, int width )
{
  if (
    dst_rect . x + dst_rect . w  >= x &&
    dst_rect . y + dst_rect . h  >= y &&
    x + width >= dst_rect . x &&
    y + height >= dst_rect . y
  )
  {
    return true;
  }
  return false;
}

void Entity::set_renderer ( SDL_Renderer * rndr )
{
  renderer = rndr;
}

void Entity::load_texture ()
{
  if ( renderer == NULL )
  {
    std::cout << "could not load texture renderer is not set. Call Entity::set_renderer ()" << std::endl;
  }

  SDL_Surface * tmp_surface = IMG_Load ( texture_path.c_str() );
  texture = SDL_CreateTextureFromSurface ( renderer, tmp_surface );
  SDL_FreeSurface ( tmp_surface );
}

void Entity::draw () {
  SDL_RenderCopyEx ( renderer, texture, &src_rect, &dst_rect, angle, NULL, flip );
}


Projectile::Projectile ( int x , int y, int w, int h, const char * path, size_t angle ) 
  :Entity::Entity(x,y,w,h,path)
{
  this->angle = angle;
  this->flip = flip;
  switch (angle)
  {
  case 0:
    /* code */
    this->velocity . x = speed;
    break;
  
  case 90:
    /* code */
    this->velocity . y = speed;
    break;
  case 180:
    this->velocity . x = -speed;
    break;

  case 270:
    this->velocity . y = -speed;
    break;
  }

  active_flag = true;
}

void Projectile::set_map_size (size_t w , size_t h )
{
  map_width = w;
  map_height = h;
}

void Projectile::on_event ( SDL_Event & event ) {
  if ( event . type == SDL_USEREVENT ) {
    if ( event . user . code == 0 ) {

      size_t dt = *((size_t*)event.user.data1);
      position . x += velocity . x * dt;
      position . y += velocity . y * dt;

      dst_rect . x = static_cast<int>(position . x);
      dst_rect . y = static_cast<int>(position . y);
    }
  }
}

Player::Player ( int x, int y, int w, int h, const char * path, size_t map_w, size_t map_h ) 
  : Entity::Entity( x, y, w, h, path )
  , map_width ( map_w )
  , map_height ( map_h )
  , key_flag ( 'N' ) 
  , speed ( 0.2 )
  , cntr ( 0 )
  , can_shoot ( true )
  , reload_time ( RELOAD )
  , next_level_speed_difference ( 0.1 )
  , next_level_reload_difference ( 5 )
  {}

void Player::set_colliders ( std::vector<std::unique_ptr<Entity>> *walls, std::vector<std::unique_ptr<Entity>> * projectiles)
{
  this->walls = walls;
  this->projectiles = projectiles;
}

void Player::set_map_size (size_t w , size_t h )
{
  map_width = w;
  map_height = h;
}

void Player::up_reload_time () 
{ 
  if ( reload_time - next_level_reload_difference > 0 )
  {
    reload_time -= next_level_reload_difference; 
  }
  else
  {
    reload_time = 0; 
  }
  
}

void Player::up_speed ()
{
  speed += next_level_speed_difference;
}

void Player::on_event ( SDL_Event & event) 
{
  if ( event . type == SDL_KEYDOWN )
  {
      switch ( event . key . keysym . sym ) 
      {
          case SDLK_w:
              if ( key_flag == 'w' || key_flag == 'N') {
                  key_flag = 'w';
                  angle = 270;
                  velocity . y = -speed ;
              }
              break;
          case SDLK_a:
              if ( key_flag == 'a' || key_flag == 'N') {
                  key_flag = 'a';
                  angle = 180;
                  velocity . x = -speed ;
              }
              break;
          case SDLK_d:
              if ( key_flag == 'd' || key_flag == 'N') {
                  key_flag = 'd';
                  angle = 0;
                  velocity . x = speed ;
              }
              break;
          case SDLK_s:
              if ( key_flag == 's' || key_flag == 'N') {
                  key_flag = 's';
                  angle = 90;
                  velocity . y = speed ;
              }
              break;

          default:
              break;
      }
  }


  if ( event . type == SDL_KEYUP ) {
      switch ( event . key . keysym . sym ) 
      {
          case SDLK_w:
              key_flag = 'N';
              velocity . y = 0;
              break;
          case SDLK_a:
              key_flag = 'N';
              velocity . x = 0;
              break;
          case SDLK_d:
              key_flag = 'N';
              velocity . x = 0;
              break;
          case SDLK_s:
              key_flag = 'N';
              velocity . y = 0;
              break;
          case SDLK_SPACE:
          //  shoot
              if ( can_shoot ) {
                int start_x = dst_rect . x + dst_rect . w/2;
                int start_y = dst_rect . y + dst_rect . h/2;
                std::unique_ptr <Projectile> tmp = std::make_unique<Projectile> (start_x, start_y, PROJECTILE_WIDTH, PROJECTILE_HEIGHT, "../data/projectile.png", angle);
                tmp -> set_renderer (renderer);
                tmp -> load_texture ();
                tmp -> set_map_size( map_width, map_height );
                projectiles -> emplace_back ( std::move(tmp) );
                can_shoot = false;
              }

          // proste jen pushnes projektil do projektilu
              break;
          default:
              break;
      }
  }

  if ( event . type == SDL_USEREVENT ) {
    if ( event . user . code == 0 ) {


      if ( ! can_shoot ) 
      {
        if ( cntr > reload_time ) 
        {
          can_shoot = true;
          cntr = 0;
        }
        ++cntr;
      }

      size_t dt = *((size_t*)event.user.data1);

      position . x += velocity . x * dt;
      position . y += velocity . y * dt;

      bool intersect_flag = false;
      for ( auto & w : *walls ) {
        if( w -> intersects ( position . x, position . y, width, height ) )
        {
          position . x = dst_rect . x ;
          position . y = dst_rect . y ;
          intersect_flag = true;
          break;
        }
      }

      if ( position . x < 0 || position . x + width > map_width || position . y < 0 || position . y + height > map_height )
      // if ( player is out of map )
      {
        position . x = dst_rect . x ;
        position . y = dst_rect . y ;
        intersect_flag = true;
      }

      if ( ! intersect_flag ) {
        dst_rect . x = static_cast<int>(position . x);
        dst_rect . y = static_cast<int>(position . y);
      }

      int * player_position_event_x = new int ( dst_rect . x ) ;
      int * player_position_event_y = new int ( dst_rect . y ) ;

      SDL_Event player_position_event;
      player_position_event . type = SDL_USEREVENT;

      player_position_event . user . code = 2;
      player_position_event . user . data1 = (void*)player_position_event_x;
      player_position_event . user . data2 = (void*)player_position_event_y;
      
      SDL_PushEvent ( & player_position_event );

    }
  }
}

void Enemy::set_colliders (std::vector<std::unique_ptr<Entity>> * walls)
{
  this->walls = walls;
}


void Enemy::on_event ( SDL_Event & event ) {
  if ( event . type == SDL_USEREVENT ) {
    if ( event . user . code == 2 ) {
      player_x = *static_cast<int*>(event.user.data1);
      player_y = *static_cast<int*>(event.user.data2);
      //set velocity

      src_rect.x = src_rect.w * (static_cast<int>(SDL_GetTicks() + position.x  / frame_rate) % frames);

      if ( dst_rect . x < player_x )
      {
        velocity . x = speed;
      }
      else
      {
        velocity . x = -speed;
      }

      if ( dst_rect . y > player_y )
      {
        velocity . y = -speed;
      }
      else
      {
        velocity . y = speed;
      }
      
      known_player_position = true;
    }
  }

  if ( event . type == SDL_USEREVENT )
  {
    if ( event . user .  code == 0 )
    {
      //jdi smerem k cili tzn nejdriv vem blizsi souradnici a tu priblizuj
      if ( known_player_position )
      {
        size_t dt = *((size_t*)event.user.data1);

        position . x += velocity . x * dt;
        position . y += velocity . y * dt;

        bool intersect_flag = false;
        for ( auto & w : *walls ) {
          if( w -> intersects ( position . x, position . y, width, height ) )
          {
            position . x = dst_rect . x ;
            position . y = dst_rect . y ;
            intersect_flag = true;
            break;
          }
        }

        if ( ! intersect_flag ) {
          dst_rect . x = static_cast<int>(position . x);
          dst_rect . y = static_cast<int>(position . y);
        }
      }
    }
  }
}

void Wall::on_event ( SDL_Event & event ) 
{
  if ( event . type == SDL_USEREVENT )
  {
    if ( event . user .  code == 0 )
    {
      src_rect.x = src_rect.w * static_cast<int>((SDL_GetTicks() / frame_rate) % frames);
    }
  }
}

void Bonus::on_event ( SDL_Event & event ) 
{
  if ( event . type == SDL_USEREVENT )
  {
    if ( event . user .  code == 0 )
    {
      src_rect.x = src_rect.w * static_cast<int>((SDL_GetTicks() / frame_rate) % frames);
    }
  }
}

void Banner::init ( int x, int y, int w, int h, const char * path )
{
  position . x = x;
  position . y = y;
  width = w;
  height = h;

  src_rect . x = 0;
  src_rect . y = 0;
  src_rect . w = w;
  src_rect . h = h;

  dst_rect . x = x;
  dst_rect . y = y;
  dst_rect . w = w;
  dst_rect . h = h;

  angle = 0;

  flip = SDL_FLIP_NONE;

  texture_path = path;
}

void Banner::on_event ( SDL_Event & e ) {}

void Banner::delay ( uint32_t ms)
{
  SDL_Delay (ms);
}