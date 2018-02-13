#include <iostream>
#include <cstdint>
#include "SDL2/SDL.h"
#include "fontset.hpp"
#include "util/hexdump.hpp"

#define WINDOW_HEIGHT 512
#define WINDOW_WIDTH 1024
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define MAX_MEMORY_SIZE 2048


uint32_t memory[MAX_MEMORY_SIZE];

/*
  binary
  0xF0, 0x90, 0x90, 0x90, 0xF0 
    11110000
    10010000
    10010000
    10010000
    11110000
*/

void loadScreenPixels() {
  uint8_t pixels[] = { 0xF0, 0x90, 0x90, 0x90, 0xF0 };

  for (int y = 0; y < 5; y++)
  {
    uint8_t pixel = pixels[y];
    for (int x = 0; x < 8; x++)
    {
      // if the bit is on, draw to the screen
      if (pixel & 1)
      {
        int row = SCREEN_WIDTH * (y + 1);
        memory[ row + x ] = ((0x00FFFFFF * 0x01) | 0xFF000000);
      }
      pixel >>= 1;
    }
  }
}

int
main( const int argc, const char **argv )
{
  SDL_Event event;
  SDL_Renderer *renderer;
  SDL_Texture* sdlTexture;
  SDL_Window *window;
  int i;

  SDL_Init(SDL_INIT_EVERYTHING);

  window = SDL_CreateWindow("SDL2 Display Demo",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

  // create a renderer with the default driver, no flags
  renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_RenderSetLogicalSize(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);

  // create a texture layer, scaling to 64x32px
  sdlTexture = SDL_CreateTexture(renderer,
      SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STREAMING,
      SCREEN_WIDTH, SCREEN_HEIGHT);

  loadScreenPixels();

  // load memory into the texture layer
  //  requires 32bit data
  SDL_UpdateTexture(sdlTexture, NULL, memory, SCREEN_WIDTH * sizeof(Uint32));

  // clear the screen
  //  copy texture to renderer and display
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
  SDL_RenderPresent(renderer);

  // user input handler
  while (1)
  {
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
    {
      break;
    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return EXIT_SUCCESS;
}