#include<iostream>

#include<SDL3/SDL.h>

int main(){
  auto window = SDL_CreateWindow("MUL2026",1024,768,0);

  bool running = true;
  while(running){//main loop
    SDL_Event event;
    while(SDL_PollEvent(&event)){ // event loop
      if(event.type == SDL_EVENT_QUIT)running = false;
  
    }

  }

  SDL_DestroyWindow(window);
  return 0;
}
