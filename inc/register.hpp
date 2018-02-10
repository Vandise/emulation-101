#ifndef __REGISTERS
#define __REGISTERS 1

#include <cstdint>

//
// Create three 8-bit registers: a, b, c
//
struct Register {
  uint8_t value;
};

uint8_t registerGetLowerNybble(Register r)
{
  return (r.value & 0x0F);
}

uint8_t registerGetUpperNybble(Register r)
{
  return ((r.value & 0xF0) >> 4);
}

#endif