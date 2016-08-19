#ifndef _H_CHIP8
#define _H_CHIP8

#include <fstream>
#include <cstdint>
#include <string>
#include <cstdlib>
#include <time.h>
#include <iostream>

#include "font_set.hpp"

// The CHIP-8 spec defines 16 Registers
//   V0-V14  <- data registers
//   V15     <- Carry register
#define NUM_REGISTERS 16
// The CHIP-8 spec defines 4096 1-byte memory locations
//   0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
//   0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
//   0x200-0xFFF - Program ROM and work RAM
#define MEMORY_SIZE 4096
// The graphics of the CHIP-8 are black and white and the screen has a total of 2048 pixels (64 x 32)
// TODO: Replace where each pixel is a bit (a byte is 8 pixels in a row)
#define GRAPHICS_WIDTH 64
#define GRAPHICS_HEIGHT 32
#define GRAPHICS_SIZE 64 * 32
// The CHIP-8 spec defines a maximum stack depth of 16 frames.
#define STACK_SIZE 16
// The CHIP-8 spec defines a hex based keypad (0x0-0xF).
#define KEYPAD_SIZE 16
// CHIP-8 spec says program starts at 0x200
#define PROGRAM_START 0x200
// Combines two 1 byte sequences into a 2 byte sequence
#define BIT8TO16(A,B) ((A) << 8 | (B));

class CHIP8 {
private:
	/* Current operator code */
	uint16_t opcode;
	/* Index Register: Used with several memory opcodes */
	uint16_t index;
	/* Program Counter: Used to keep track of location in code */
	uint16_t program_counter;
	/* 1 Byte memory locations */
	uint8_t  memory[MEMORY_SIZE];
	/* The processor registers */
	uint8_t  registers[NUM_REGISTERS];	
	/* Delay registers, count at 60Hz. When set >0, count down to 0 */
	uint8_t  timer_delay;
	uint8_t  timer_sound; // Buzzer sounds when timer_sound reaches 0
	/* The stack for tracking location when in subroutines */
	uint16_t stack[STACK_SIZE];
	uint8_t  stack_pointer;
	/*******************************
	* Executes the next opcode in memory
	*******************************/
	void executeOpcode();
public:	
	/* Does display need to be redrawn */
	uint8_t drawFlag;
	/* Does buzzer need to play */
	uint8_t beepFlag;
	/* Black and white pixel display */
	uint8_t display[GRAPHICS_SIZE];
	/* Buttons on keypad */
	uint8_t keypad[KEYPAD_SIZE];
	/*******************************
	* Creates a new CHIP-8 emulator
	*******************************/
	CHIP8();
	~CHIP8(){}
	/*******************************
	* Resets the emulator to a clean state
	*******************************/
	void reset();
	/*******************************
	* Preforms a cycle on the chip
	*******************************/
	void cycle();
	/*******************************
	* Loads a file into memory for emulation.
	* @param file_path Path to the file that is being loaded
	*******************************/
	void loadProgram(std::string file_path);
};

#endif
