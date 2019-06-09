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
#include <random>

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

    explicit Entity ();
    explicit Entity ( int x, int y, int w, int h, const char * path );

    Vector2 get_position ();

    Vector2 get_width_height ();

    void set_position ( int x, int y );

    virtual bool intersects ( Entity * e );

    virtual bool intersects ( int x, int y, int height, int width );

    void set_renderer ( SDL_Renderer * rndr );

    void load_texture ();
    
    virtual void draw ();
                    
    virtual void on_event ( SDL_Event & event ) = 0;

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

    void set_map_size (size_t w , size_t h );

    Projectile ( int x , int y, int w, int h, const char * path, size_t angle ); 

    virtual void on_event ( SDL_Event & event );
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

public:
  explicit Player ( int x, int y, int w, int h, const char * path, size_t map_w, size_t map_h );

  void set_colliders ( std::vector<std::unique_ptr<Entity>> *walls, std::vector<std::unique_ptr<Entity>> * projectiles );

  void set_map_size (size_t w , size_t h );

  void up_reload_time ();

  void up_speed ();

  virtual void on_event ( SDL_Event & event ) override;
};

class Enemy : public Entity 
{
  int player_x; 
  int player_y;
  double speed = 0.3;
  bool known_player_position = false;
  using Entity::Entity;
  std::vector<std::unique_ptr<Entity>> * walls;
  unsigned short frames = 8;
  unsigned short frame_rate = 50;

public:
  void set_colliders ( std::vector<std::unique_ptr<Entity>> * walls );

  virtual void on_event ( SDL_Event & event );
};

class Wall : public Entity
{
  public:
    using Entity::Entity;
    unsigned short frames = 8;
    unsigned short frame_rate = 200;

    virtual void on_event ( SDL_Event & event );
};

class Bonus : public Entity
{
  public:
    using Entity::Entity;
    unsigned short frames = 4;
    unsigned short frame_rate = 100;
    virtual void on_event ( SDL_Event & event );
};

class Banner : public Entity
{
  public:
    void init ( int x, int y, int w, int h, const char * path );
    virtual void on_event ( SDL_Event & e );

    void delay ( uint32_t ms );
};