#include <iostream>
#include <map>
#include "register.hpp"
#include "util/hexdump.hpp"

#define MEMORY_SIZE 0xFF
#define PROGRAM_START_ADDRESS 1
#define LD(v) memory[*programCounter] = v; *programCounter += 1;

// pointer to instruction functions
typedef void(*instructionHandle)();

// emulated memory
uint8_t memory[MEMORY_SIZE];

// address 0 is our program counter (the current instruction being executed
uint8_t *programCounter = &memory[0];

// registers
Register a;
Register b;
Register c;

// instruction handler
std::map<uint8_t, instructionHandle> instructions;

//
// increments program counter by one
//
void incrementProgramCounter() {
  *programCounter += 1;
}

//
// instruction 0x01
//  mov a, value
//
void movRegA()
{
  incrementProgramCounter();
  uint8_t value = memory[*programCounter];
  a.value = value;
  incrementProgramCounter();
}

//
// Sets instruction handlers
//
void setupCPU()
{
  instructions[0x01] = &movRegA;
}

//
// resets the program counter to the starting address
//
void resetProgramCounter()
{
  *programCounter = (uint8_t)PROGRAM_START_ADDRESS;
}

//
// cycle one instruction
//  note some instructions may take more then 1 cpu cycle
//  most use thead.sleep or some form of delay to prevent
//  fast executions
//
void cycle() {
  uint8_t instruction = memory[*programCounter];
  if ( instructions.count(instruction) )
  {
    (*(instructions[instruction]))();
  }
  else
  {
    std::cout << "Unimplemented instruction: " << std::to_string(instruction) << std::endl;
  }
}

int
main( const int argc, const char **argv )
{

  setupCPU();

  // clear the registers
  a.value = 0x00;
  b.value = 0x00;
  c.value = 0x00;

  resetProgramCounter();

  // load instruction 1
  //    set register a to a value
  LD(0x01);
  LD(0xFF);

  std::cout << "program counter: " << (int)*programCounter << std::endl;
  std::cout << hexdump(memory) << std::endl;

  resetProgramCounter();

  // run 0x01
  //    with parameter 0xFF (255)
  cycle();

  std::cout << "Register A: " << (int)a.value << std::endl;

  return 0;
}