autoscale: true

# Emulation 101
## Reconstructing the Matrix

![inline](https://imgs.xkcd.com/comics/pong.png)

---

# Who?

- Benjamin J. Anderson
- Software Engineer at Mashable.com
- Teacher at CVTC
- Student - Data Science
- I build things
	- Usually pointless and serve no purpose other then being cool
- github.com/Vandise
-  @BJS_Anderson | vandise.com

^ I'm passionate about data and extracting/constructing information out of it.

^ We'll be looking at some binary and hex values. They have no meaning without context. When given context, we end up with computer software.

^ data is not information!

---

# What?

- Hardware
	- Display, Memory, Registers, Processor
- Bitwise Operations
	- Endianness 
	- &, |, <<, >>, ^, ~ 
- Chip 8 Instruction Set
- Emulation

---

# Why?

* You get to build the matrix in which programs will run
* Master of bitwise operations
* Better understanding of hardware
* Support legacy software without the hardware
	* PS3 supports PS1
	* Windows Subsystem for Linux supports Windows10 (.NET core)
* You get to turn data into information

---
# Before we begin...

![inline](https://imgs.xkcd.com/comics/ballmer_peak.png)

---
# For those who want to follow along

Code can be found here:

- https://github.com/Vandise/Chip8
- https://github.com/Vandise/emulation-101

These slides use branches in the emulation-101 repository.

Mac, Linux, or WSL only. Windows if you tweek the Makefile... maybe.

---

# [fit] What is a program?

```c
0000  f0 90 90 90 f0 20 60 20 20 70 f0 10 f0 80 f0 f0
0010  10 f0 10 f0 90 90 f0 10 10 f0 80 f0 10 f0 f0 80
0020  f0 90 f0 f0 10 20 40 40 f0 90 f0 90 f0 f0 90 f0
0030  10 f0 f0 90 f0 90 90 e0 90 e0 90 e0 f0 80 80 80
0040  f0 e0 90 90 90 e0 f0 80 f0 80 f0 f0 80 f0 80 80
```

^ Demo branch 1.program

---

# Hardware - Memory

* A series of on-off switches in *8 bit* chunks (*1 byte*)
* Each byte is given a unique *address*, can be chained together
* Chip8 supports up to 4096 *bytes* of memory
* Programs are loaded into memory and start at *address* 512

![inline](http://dsearls.org/courses/C391OrgSys/Memory/Seq11.gif)

^ Reference branch 1.program on how memory can be addressed.
^ At address 0010 we have 0x10F0

---

# Hardware - Registers

* Memory built into a processor, faster then RAM
* Broken into General and Special purpose registers
* Chip8 has 16, *8-bit* general-purpose, registers called *Vx* (any data can be placed in them)
	* *x* is 0 - F (1 - 16)
* Register *VF*(16) is used for flags, math overflow, errors, etc.

^ branch 2.registers
^ demo how registers operate like memory

---

# Hardware - Registers (Cont)

* One *16-bit* register used for memory addressing (called *I*)
* One *16-bit* register containing the program counter (address position)
* One *8-bit* register containing the stack-pointer

---

# Hardware - Processor

* The brain of the computer
* Executes instructions and interacts with hardware

This is the core of our emulator, a piece of software that loads a program into *memory*, executes instructions and interprets what it should do. Update a register, read a memory address, display a sprite, etc.

^ branch 3.basic-cpu
^ demo executing instructions, setting a register value

---

# Hardware - Display

* Some sort of visual output. Monitors, TVs, LEDs.
* Atari had no video ram! RF channel 3 or 4 (NTSC/Analog)
* Rendered with Integrated Graphics, Graphics Cards
	* Display information is wrote to dedicated video memory
	* It is then rendered by the display hardware (PPU)
	* *Note that this is a extremely simplified version*
	* Chip8 uses a 64x32 pixel monochrome display (2kb)

^ 4.display
^ SDL and bit rendering with textures

---

# Questions this far?

*We will be going over Chip8 in much more detail, this was a brief introduction into the hardware we have to emulate*

Congratulations! You build your first, fragmented, emulator!

^ explain demo of mini emulator after bit operations

---

# Maths - Bitwise Operations

Manipulates *bits* in a byte with logical operations.

Chip8 instructions are 2 bytes (16-bits) long and broken into *4-bit* (*1-nybble*) chunks for interpretation. They can be combined to make a max *12-bit* value.

The following is a Chip8 instruction we need to parse:

```c
	// set register 2 to contain the value 16
	0x6210
	// OpCode: 0x6nvv - Set Register n to vv
	// However, 0x0200 is not "register 2", it's 512
	// It needs to be translated to a value of 1 - 16 by shifting it to the lower 4 bytes
```

---

# Maths - Logical AND ( & )

Performs a truthy check against each bit in a byte.

```c

     11001000  
   & 10111000 
     -------- 
   = 10001000
```

If both bits are 1, 1 is returned. If one or more bits are 0, 0 will be returned.

---

# Maths - Logical AND ( & ) Cont

```c
	// logical & with hex
	  0x6210  // 0110 0010 0001 0000
	& 0x0F00  // 0000 1111 0000 0000
	  ------
	= 0x0200 (512)
```

---

# Maths - Right Shift ( >> )

Shifts the first truthy bit a specified amount of bytes to the right.

```c
// 0x0200
   0010 0000 0000 >> 8
   ------
=  0000 0000 0010 ( 2 )
```

---

# Maths - Logical OR ( | )

Similar to bitwise AND, bitwise OR only operates at the bit level. Its result is a 1 if one of the either bits is 1 and zero only when both bits are 0.

```c
      11001000  
    | 10111000 
      -------- 
    = 11111000
```

---

# Maths - Logical OR ( | ) Cont

Before we can parse the instruction, we first need to *construct* it. Each instruction is 2 bytes:

```c

// construct the following instruction from two separate bytes:
// 0x62 0x10

uint16_t instruction;   // 0000 0000 0000 0000 ; 0x0000

instruction = 0x62 << 8; 
// => 0x6200

instruction = instruction | 0x10;
// => 0x6210

```

---

# Maths - Endianness

Endianness is the order of bytes and which is considered the "most" or "least" significant bit (positive/negative/highest-value). *See two's complement for more details*. 

```c
  
    Two's Complement Example:
      0x6a02
    Big Endian:
      low ---> high
        02   6a
    Mask:
      0x026a & 0xF000
          ^      ^
          |------|
      0x026a & 0xF000
        ^          ^
        |----------|  
  
```

^ demo branch 5.mini-emulator
^ instructions are two bytes

---

# The Chip8 Matrix

Chip-8 is a simple, interpreted, programming language which was first used on some do-it-yourself computer systems in the late 1970s and early 1980s. These computers typically were designed to use a television as a display, had between 1 and 4K of RAM, and used a 16-key hexadecimal keypad for input.

```c
// Example Chip8 program:
// Prints the number "0" to the screen

0x61 0x00 0xF1 0x29 0x62 0x10
0x63 0x05 0xD2 0x35 0x12 0x0A

```

^ chip8, ./bin/chip8 resources/pong --test

---

# Program Breakdown

```c
// ld_vx_byte
// - set register 1 to 0 (the single digit hex value to print)
0x61 0x00

// fx_ld_f_vx
// - index register points to register 1
0xF1 0x29

// ld_vx_byte
// - set register 2 to 16 ( the x position )
0x62 0x10

// ld_vx_byte
// - set register 3 to 5 ( the y position )
0x63 0x05

// drw_vx_vy_nibble
// - draw x value in reg 2, y value in reg 3, the number of pixels ( 5 )
// - index pointer contains register value
0xD2 0x35

// loop start
0x12 0x0A

```

---

# Chip 8 Memory Layout

```c
+---------------+= 0xFFF (4095) End of Chip-8 RAM
|               |
|               |
|               |
|               |
|               |
| 0x200 to 0xFFF|
|     Chip-8    |
| Program / Data|
|     Space     |
|               |
|               |
|               |
|               |
|               |
+---------------+= 0x200 (512) Start of most Chip-8 programs
| 0x000 to 0x1FF|
| Reserved for  |
| Fonts/Data    |
+---------------+= 0x000 (0) Start of Chip-8 RAM

```

---

# Chip 8 Program In Memory

```c
// fonts: addr 0x000 - 0x200
0000  f0 90 90 90 f0 20 60 20 20 70 f0 10 f0 80 f0 f0
0010  10 f0 10 f0 90 90 f0 10 10 f0 80 f0 10 f0 f0 80
0020  f0 90 f0 f0 10 20 40 40 f0 90 f0 90 f0 f0 90 f0
0030  10 f0 f0 90 f0 90 90 e0 90 e0 90 e0 f0 80 80 80
0040  f0 e0 90 90 90 e0 f0 80 f0 80 f0 f0 80 f0 80 80
// ~~~
// Program: addr 0x200 - 0x0ff0
0200  61 00 f1 29 62 10 63 05 d2 35 12 0a 00 00 00 00
// ...
```

---

# Fonts

Fonts are 5 byte sprites, represented by truthy bits:

```c

// Hex
0xF0 0x90 0x90 0x90 0xF0

// Binary
11110000
10010000
10010000
10010000
11110000

// False bits removed
****
*  *
*  *
*  *
****

```

---

# Emulating Memory

Chip8 has 8-bit registers and supports some "special" registers which are simply two registers combined together to create 16-bit registers. Chip8 is an *8-bit* machine, so we parse our program in 1 byte chunks and load each byte into memory.

```c
uint8_t  memory[4096]; // reserve 4kb of emulated memory
```

Note that our program starts at address 512 (0x200) in memory!

---

# Loading our Program into Memory

Once we have our memory array defined, we can populate it with our program:

```c

#define LD(byte) memory[512 + offset] = byte; offset++;

int offset = 0;
LD(0x61); LD(0x00);
LD(0xF1); LD(0x29);
LD(0x62); LD(0x10);
LD(0x63); LD(0x05);
LD(0xD2); LD(0x35);
LD(0x12); LD(0x0A);

```

---

# Emulating Registers and Stack

The registers and stack operate similar to memory, we're just limited to the amount of data that can be stored ( 1 byte in registers, 2 in the stack ).

```c
// initialize 16 registers, register 0 is not used
uint8_t  registers[16];

// the stack can hold 2 bytes, but is limited to 17 items -- FILO
uint16_t stack[16];
```



---

# Emulating the CPU

Emulating a CPU can be broken down into four steps:

- Run a CPU cycle (execute 1 instruction)
- Check if the video memory (buffer) has updated (if so, draw)
- Limit CPU speed, (pause the current thread)
- Repeat

---

# CPU Pseudocode

```c

while true

	chip8.cycle()
	if chip8.drawFlag
		for each address in video memory
			pixel = chip8.graphicsBuffer[address]
			convert pixel to 32bit video memory (using SDL2)
			push to display
		end
		display.refresh()
		sleep for 100ms
	end

end

```

---

# The CPU Cycle

```c
// Parse the opCode from memory
uint16_t = opCode = memory[programCounter] << 8 | memory[programCounter + 1];
// keep track of our position in memory
programCounter += 2;

// extract the instruction
uint16_t instruction = opCode & 0xF000;

// execute the instruction
if ( instructions.count(instruction) )
{
	(instructions[instruction])();
}
else
{
	cout << "Unimplemented OpCode: " << opCode << std::endl;
	exit(1);
}

```

---

# The CPU Cycle - Instruction

```c

#define MASK(hex) ((opCode & hex))

// load a value into register x
// instruction 0x6000
void ld_vx_byte()
{
	// extract the register, shift 8 bits to the right
	// extract the value from the last byte
	registers[MASK(0x0F00) >> 8] = MASK(0x00FF);
}

// add the instructions to the hashmap
instructions[0x6000] = &ld_vx_byte;

```

---

# Displaying the Screen

For emulating a monochrome screen, SDL is an excellent library to support this. It accepts one parameter after being configured: a vector of bits. These bits and their position represent where a pixel will be filled in.

Whenever the CPU sets a drawFlag, we update the screen:

```c
// pixelBuffer is our vector of bits
SDL_UpdateTexture(sdlTexture, NULL, pixelBuffer);
```

---

# Demo - Pong!

Requires SDL2. Note: not all instructions have been implemented, Pong is the only game tested.
Currently have not implemented user controls.

```bash

$ git clone https://github.com/Vandise/Chip8
$ cd Chip8
$ make
$ ./bin/c8 resources/pong

# for our test program to draw 0 to the screen
$ ./bin/c8 resources/pong --test

```

---

# Reflections

* Endianness can really screw up your emulator!
	* Some Chip8 programs use different Endianness or chose not to use Two's Complement
	* It made debugging with other games a challenge
* It was amazing on how storage conscious some programmers were in the 1970s. Not a single *bit* is wasted in the Chip8 instruction set
* Pong isn't smart enough to question the matrix in which I put it in

---

# Resources

Endianness, bytecode parsing, and compilation:

* https://github.com/Vandise/bytecode-instructions-demo

Chip8 Instruction Reference:

* http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

---

# Stuff I'm doing

* https://github.com/Vandise/regole
	* Web-based asm simulator (tutorial-driven) and uses motion
* https://github.com/Vandise/motion
	* Parliamentary Procedure-influenced, condition-driven, web notification utility
* https://github.com/Vandise/z80
	* Z80 processor emulator. Utilized for educational purposes to demonstrate unit testing and engineering concepts.