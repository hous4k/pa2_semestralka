#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <iterator>
#include "Vector2.h"

#define PROJECTILE_HEIGHT 5
#define PROJECTILE_WIDTH 10

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
  std::vector<std::unique_ptr<Entity>> *enemies;
  std::vector<std::unique_ptr<Entity>> *projectiles;

  public:
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
  }

  virtual void on_event ( SDL_Event & event ) {
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
};

class Player : public Entity
{
  size_t map_width;
  size_t map_height;
  char key_flag = 'N';
  float speed = 0.2;
  std::vector<std::unique_ptr<Entity>> *walls;
  std::vector<std::unique_ptr<Entity>> *projectiles;
  std::vector<std::unique_ptr<Entity>> *enemies;


public:
  explicit Player ( int x, int y, int w, int h, const char * path, size_t map_w, size_t map_h ) 
    : Entity::Entity( x, y, w, h, path )
    , key_flag ( 'N' ) 
    , map_width ( map_w )
    , map_height ( map_h )
    {}

  void set_colliders ( std::vector<std::unique_ptr<Entity>> *walls, std::vector<std::unique_ptr<Entity>> * projectiles, std::vector<std::unique_ptr<Entity>> * enemies )
  {
    this->walls = walls;
    this->projectiles = projectiles;
    this->enemies = enemies;
  }
  void set_map_size (size_t w , size_t h )
  {
    map_width = w;
    map_height = h;
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
              {
                int start_x = dst_rect . x + dst_rect . w/2;
                int start_y = dst_rect . y + dst_rect . h/2;
                std::unique_ptr <Projectile> tmp = std::make_unique<Projectile> (start_x, start_y, PROJECTILE_WIDTH, PROJECTILE_HEIGHT, "projectile.png", angle);
                tmp -> set_renderer (renderer);
                tmp -> load_texture ();
                projectiles -> emplace_back ( std::move(tmp) );
              }

            // proste jen pushnes projektil do projektilu
                break;
            default:
                break;
        }
    }
    if ( event . type == SDL_USEREVENT ) {
      if ( event . user . code == 0 ) {

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
        // if ( player is not hiting a wall )
        {
          position . x = dst_rect . x ;
          position . y = dst_rect . y ;
          intersect_flag = true;
        }

        if ( ! intersect_flag ) {
          dst_rect . x = static_cast<int>(position . x);
          dst_rect . y = static_cast<int>(position . y);
        }

      }
    }
  }
};

class Enemy : public Entity 
{
  using Entity::Entity;
  virtual void on_event ( SDL_Event & e ) {}
};

class Wall : public Entity
{
  using Entity::Entity;
  virtual void on_event ( SDL_Event & e ) {}
};


/*
class Projectile : public Entity
{
  void set_position ( int x, int y )
  {
    position_x = x;
    position_y = y;
  }

};

class Gun : public Entity
{
  void set_position ( int x, int y )
  {
    position_x = x;
    position_y = y;
  }

};
*/