
#include <iostream>
#include "Game.h"

//Uint32 MY_EVENT_TYPE; 

using namespace std;

// argc args required by SDL
int main ( int argc, char * args [])
{
  const size_t FPS = 60;
  // frame exposure time in milliseconds so 1000 / FPS is a period of frame wiht 60fps in millisecon
  const size_t * frame_delay = new size_t ( 1000 / FPS );

  uint32_t frame_start;
  unsigned int frame_time;

  size_t last_frame = 0; 
  //Uint32 MY_EVENT_TYPE;
  //MY_EVENT_TYPE = SDL_RegisterEvents(1);
  SDL_Event custom_event;
  //if ( MY_EVENT_TYPE != ((Uint32)-1)) {
    custom_event . type = SDL_USEREVENT; //MY_EVENT_TYPE;
    // code of timer
    custom_event . user . code = 0;
    custom_event . user . data1 = (void*)frame_delay;
    cout << "custom event created" << endl;
  /*} else {
    cout << "!!!custom event was not created!!!" << endl;
    return 0;
  }
  */

  Game game ( "semestralka", false );

  int cnt = 0;
  while ( game . running() ) {

    // millisiseconds since initialization

    game . handle_events ();
    game . collisions ();

    frame_start = SDL_GetTicks ();
    if ( *frame_delay < ( frame_start - last_frame ) )
    {
      game . render ();
      last_frame = SDL_GetTicks();
      SDL_PushEvent ( & custom_event );
    }
    frame_time = SDL_GetTicks () - frame_start;

    // if computation of vents update and render has been too fast
      //SDL_Delay ( frame_delay - frame_time );
    // else unfortunately we can not speed up our computational time
  }

  delete frame_delay;
  //game . clean ();

  //Syscall param writev(vector[...]) points to uninitialised byte(s)
  // problem with X11

}

