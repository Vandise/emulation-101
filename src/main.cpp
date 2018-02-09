#include <iostream>
#include <string>
#include <cstdint>
#include <fstream>
#include "util/hexdump.hpp"

// 50kb
#define MAX_PROGRAM_SIZE 0xC350

// simulate 50kb of RAM
uint8_t memory[MAX_PROGRAM_SIZE];

// when a program is executed, it is first loaded into memory
void loadProgramIntoMemory(const std::string fileName) {
  std::ifstream file(fileName, std::ios::binary);
  if ( file.tellg() <= MAX_PROGRAM_SIZE && !!file)
  {
    char byte;
    int i = 0;
    while( file.get(byte) )
    {
      memory[i] = (uint8_t)byte;
      i++;
    }
  }
}

int
main( const int argc, const char **argv )
{
  loadProgramIntoMemory("resources/chess.exe");

  // dump memory to the screen
  std::cout << hexdump(memory) << std::endl;
  return 0;
}