#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <iostream>
#include <string>
#include "Vector2.h"

class Entity {
  protected:
    Vector2 position;
    Vector2 velocity;
    int position_x,position_y, width,height;
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

                    
    virtual void on_event ( SDL_Event & e ) = 0;

    //virtual void on_event () = 0;
};

class Player : public Entity
{
  bool next_weapon_flag = false;
  bool prev_weapon_flag = false;
  char key_flag = 'N';

public:
  explicit Player ( int x, int y, int w, int h, const char * path ) 
    : Entity::Entity( x, y, w, h, path )
    , next_weapon_flag ( false )
    , prev_weapon_flag ( false )
    , key_flag ( 'N' ) 
    {}

  virtual void on_event ( SDL_Event & event ) override
  {
    if ( event . type == SDL_KEYDOWN )
    {
        switch ( event . key . keysym . sym ) 
        {
            case SDLK_w:
                if ( key_flag == 'w' || key_flag == 'N') {
                    key_flag = 'w';
                    angle = 270;
                    velocity . y = -1 ;
                }
                break;
            case SDLK_a:
                if ( key_flag == 'a' || key_flag == 'N') {
                    key_flag = 'a';
                    angle = 180;
                    velocity . x = -1 ;
                }
                break;
            case SDLK_d:
                if ( key_flag == 'd' || key_flag == 'N') {
                    key_flag = 'd';
                    angle = 0;
                    velocity . x = 1 ;
                }
                break;
            case SDLK_s:
                if ( key_flag == 's' || key_flag == 'N') {
                    key_flag = 's';
                    angle = 90;
                    velocity . y = 1 ;
                }
                break;

            case SDLK_e:
                //previous weapon
                if ( ! prev_weapon_flag && ! next_weapon_flag)
                {
                    next_weapon_flag = true;
                }
                break;
            case SDLK_q:
                //next weapon
                if ( ! prev_weapon_flag && ! next_weapon_flag)
                {
                    prev_weapon_flag = true;
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
            case SDLK_e:
                //previous weapon
                next_weapon_flag = false;
                break;
            case SDLK_q:
                //next weapon
                prev_weapon_flag = false;
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

        dst_rect . x = position . x;
        dst_rect . y = position . y;

        dst_rect . w = width;
        dst_rect . h = height;
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