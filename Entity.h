#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <iterator>
#include "Vector2.h"

#define PROJECTILE_HEIGHT 5
#define PROJECTILE_WIDTH 10

#define RELOAD 20

class Entity {
  protected:
    Vector2 position;
    Vector2 velocity;
    int width,height;
    SDL_Texture * texture;
    SDL_Rect src_rect;
    SDL_Rect dst_rect;
    size_t angle;
    SDL_RendererFlip flip;
    SDL_Renderer * renderer;
    std::string texture_path; 
  public:

    explicit Entity ( int x, int y, int w, int h, const char * path ) 
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

    Vector2 get_position ()
    {
      return position;
    }
    void set_position ( int x, int y )
    {
      position . x = x;
      position . y = y;

      dst_rect . x = x;
      dst_rect . y = y;
    }

    bool intersects ( Entity * e )
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

    bool intersects ( int x, int y, int height, int width )
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

    void set_renderer ( SDL_Renderer * rndr )
    {
      renderer = rndr;
    }

    void load_texture ()
    {
      if ( renderer == NULL )
      {
        std::cout << "could not load texture renderer is not set. Call Entity::set_renderer ()" << std::endl;
      }

      SDL_Surface * tmp_surface = IMG_Load ( texture_path.c_str() );
      texture = SDL_CreateTextureFromSurface ( renderer, tmp_surface );
      SDL_FreeSurface ( tmp_surface );
    }

    void draw () {
      SDL_RenderCopyEx ( renderer, texture, &src_rect, &dst_rect, angle, NULL, flip );
      //SDL_RenderCopy ( renderer, texture, &src_rect, &dst_rect );
    }

                    
    virtual void on_event ( SDL_Event & event ) = 0;

    //virtual void on_event () = 0;
};

class Projectile : public Entity
{

  double speed = 1;
  std::vector<std::unique_ptr<Entity>> *walls;
  std::vector<std::unique_ptr<Entity>> *projectiles;

  size_t map_width;
  size_t map_height;

  bool active_flag;

  public:

  void set_map_size (size_t w , size_t h )
  {
    map_width = w;
    map_height = h;
  }

  Projectile ( int x , int y, int w, int h, const char * path, size_t angle ) 
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

  virtual void on_event ( SDL_Event & event ) {
    if ( event . type == SDL_USEREVENT ) {
      if ( event . user . code == 0 ) {

        size_t dt = *((size_t*)event.user.data1);
        position . x += velocity . x * dt;
        position . y += velocity . y * dt;

        if ( ( position . x < 0 || position . y < 0 || position . x > map_width || position . y > map_height ) && active_flag )
        {
          //send message that you are out of bounds
          SDL_Event destroy_message;
          destroy_message . type = SDL_USEREVENT; //MY_EVENT_TYPE;
            // code of timer
          destroy_message . user . code = 5;
          destroy_message . user . data1 = (void*)this;

          SDL_PushEvent ( & destroy_message );
          std::cout << "message sent" << std::endl;
          active_flag = false;
        }

        dst_rect . x = static_cast<int>(position . x);
        dst_rect . y = static_cast<int>(position . y);
      }
    }
  }
};

class Player : public Entity
{
  size_t map_width;
  size_t map_height;
  char key_flag;
  double speed;
  unsigned short cntr;

  bool can_shoot;

  unsigned short reload_time;

  double next_level_speed_difference;
  unsigned short next_level_reload_difference;

  std::vector<std::unique_ptr<Entity>> *walls;
  std::vector<std::unique_ptr<Entity>> *projectiles;

  //enum bonus_level { RAPIDFIRE,SPEED };

public:
  explicit Player ( int x, int y, int w, int h, const char * path, size_t map_w, size_t map_h ) 
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

  void set_colliders ( std::vector<std::unique_ptr<Entity>> *walls, std::vector<std::unique_ptr<Entity>> * projectiles)
  {
    this->walls = walls;
    this->projectiles = projectiles;
  }

  void set_map_size (size_t w , size_t h )
  {
    map_width = w;
    map_height = h;
  }

  void up_reload_time () 
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

  void up_speed ()
  {
    speed += next_level_speed_difference;
  }

  virtual void on_event ( SDL_Event & event) override
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
                  std::unique_ptr <Projectile> tmp = std::make_unique<Projectile> (start_x, start_y, PROJECTILE_WIDTH, PROJECTILE_HEIGHT, "projectile.png", angle);
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
        // code of timer
        player_position_event . user . code = 2;
        player_position_event . user . data1 = (void*)player_position_event_x;
        player_position_event . user . data2 = (void*)player_position_event_y;
        
        SDL_PushEvent ( & player_position_event );

      }
    }
  }
};

class Enemy : public Entity 
{
  int player_x; 
  int player_y;
  double speed = 0.3;
  bool known_player_position = false;
  using Entity::Entity;
  std::vector<std::unique_ptr<Entity>> * walls;

public:
  void set_colliders (std::vector<std::unique_ptr<Entity>> * walls)
  {
    this->walls = walls;
  }


  virtual void on_event ( SDL_Event & event ) {
    if ( event . type == SDL_USEREVENT ) {
      if ( event . user . code == 2 ) {
        player_x = *static_cast<int*>(event.user.data1);
        player_y = *static_cast<int*>(event.user.data2);
        //set velocity

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
};

class Wall : public Entity
{
  using Entity::Entity;
  virtual void on_event ( SDL_Event & e ) {}
};

class Bonus : public Entity
{
  using Entity::Entity;
  virtual void on_event ( SDL_Event & e ) {}
};