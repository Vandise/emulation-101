#include <iostream>
#include <cstdint>
#include <map>
#include "SDL2/SDL.h"
#include "util/hexdump.hpp"

#define WINDOW_HEIGHT 512
#define WINDOW_WIDTH 1024
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define MAX_MEMORY_SIZE 4096
#define VIDEO_MEMORY_SIZE 2048
#define PROGRAM_START 0xFF

#define LD(v) memory[programCounter] = v; programCounter += 1;

typedef void(*instructionHandle)();

uint8_t  memory[MAX_MEMORY_SIZE];
uint32_t videoBuffer[VIDEO_MEMORY_SIZE];

// address 0xFF is our program counter (the current instruction being executed)
uint16_t programCounter = PROGRAM_START;

// current instruction being executed
uint16_t instruction = 0;

// instruction handler
std::map<uint8_t, instructionHandle> instructions;

// increments program counter by one
void incrementProgramCounter()
{
  programCounter += 1;
}

/*
  first 200 bytes of memory are reserved for sprites
*/
void loadSpriteIntoMemory()
{
  uint8_t sprites[] = { 0xF0, 0x90, 0x90, 0x90, 0xF0 };
  for (int i = 0; i < 5; i++)
  {
    memory[i] = sprites[i];
  }
}

//
// Instruction 0x55 - load sprite from memory address
//
void ldSpriteMemAddr()
{
  uint8_t spriteAddress = instruction & 0x00FF;
  for (int y = 0; y < 5; y++)
  {
    uint8_t pixel = memory[spriteAddress + y];
    for (int x = 0; x < 8; x++)
    {
      // if the bit is on, draw to the screen
      if (pixel & 1)
      {
        int row = SCREEN_WIDTH * (y + 1);
        videoBuffer[ row + x ] = ((0x00FFFFFF * 0x01) | 0xFF000000);
      }
      pixel >>= 1;
    }
  }
  incrementProgramCounter(); incrementProgramCounter();
}

// Sets instruction handlers
void setupCPU()
{
  instructions[0x55] = &ldSpriteMemAddr;
}

void cycle() {
  // instructions are two bytes
  instruction = memory[programCounter] << 8 | memory[programCounter + 1];
  uint8_t opCode = (instruction & 0xFF00) >> 8;

  if ( instructions.count(opCode) )
  {
    (*(instructions[opCode]))();
  }
  else
  {
    std::cout << "Unimplemented instruction: " << std::to_string(opCode) << std::endl;
  }
}

// resets the program counter after loading instructions
void resetProgramCounter()
{
  programCounter = PROGRAM_START;
}

// program to run
void loadProgram()
{
  // ldSpriteMemAddr, sprite address: 0
  LD(0x55); LD(0x00);
}

int
main( const int argc, const char **argv )
{
  SDL_Event event;
  SDL_Renderer *renderer;
  SDL_Texture* sdlTexture;
  SDL_Window *window;
  int i;

  resetProgramCounter();
  setupCPU();
  loadSpriteIntoMemory();
  loadProgram();
  resetProgramCounter();

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


  cycle();

  // load memory into the texture layer
  //  requires 32bit data
  SDL_UpdateTexture(sdlTexture, NULL, videoBuffer, SCREEN_WIDTH * sizeof(Uint32));

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