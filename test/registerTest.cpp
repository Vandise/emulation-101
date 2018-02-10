#include "test/catch.hpp"
#include "register.hpp"

SCENARIO("Registers are initialized", "[register]")
{
  Register a;
  GIVEN("the lower nybble is needed")
  {
    THEN("registerGetLowerNybble returns the lower nybble")
    {
      uint8_t value = 0x12;
      a.value = value;
      REQUIRE( registerGetLowerNybble(a) == ( value & 0x0F ) );
    }
  }

  GIVEN("the upper nybble is needed")
  {
    THEN("registerGetUpperNybble returns the upper nybble")
    {
      uint8_t value = 0x31;
      a.value = value;
      REQUIRE( registerGetUpperNybble(a) == ( (value & 0xF0) >> 4 ) );
    }
  }
}