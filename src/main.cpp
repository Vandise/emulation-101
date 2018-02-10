#include <iostream>
#include "register.hpp"

// registers are like memory, but built in to the cpu
// they are more performant then addressing memory as there's no memory bus

Register a;
Register b;
Register c;

int
main( const int argc, const char **argv )
{
  // computer sets values to the 8bit registers
  a.value = 0x11;
  b.value = 0x22;
  c.value = 0x33;

  // computer instructions can retrieve values in a register
  // or update them. We'll see how with the cpu.
  // In c or c++ we can do:
  a.value += b.value;

  // but there's a few extra steps with the CPU.
  std::cout << "The value of register a is: " << (int)a.value << std::endl;

  return 0;
}