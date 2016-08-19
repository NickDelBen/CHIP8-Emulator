#include "chip8.hpp"

CHIP8::CHIP8() {
	this->reset();
}

// Loads a file into memory for emulation.
void CHIP8::loadProgram(std::string file_path) {
	std::streampos start;
	std::streampos end;
	int file_length;
	int bytes_read;
	uint8_t * location_start;
	char * write_pointer;

	std::ifstream file_input (file_path.c_str(), std::ios::binary);
	// Find start location of file
	file_input.seekg(0, file_input.end);
	file_length = file_input.tellg();
	file_input.seekg(0, file_input.beg);
	// Ensure the file is the correct size
	if (file_length > (MEMORY_SIZE - PROGRAM_START)) {
		std::cout << "File " << file_path << " (" << file_length << ") larger than avaiable CHIP8 memory (" 
		<< MEMORY_SIZE - PROGRAM_START << ")" << std::endl;
		return;
	}
	// Read the file into CHIP8 memory
	write_pointer = (char *) &(this->memory[PROGRAM_START]);
	file_input.read(write_pointer, file_length);
}

// Preforms a cycle on the chip
void CHIP8::cycle() {
	// Fetch opcode from memory
	this->opcode = BIT8TO16(memory[this->program_counter],memory[this->program_counter + 1])

	// Execute the opcode
	this->executeOpcode();
	
	// Update delay timer
	if (this->timer_delay > 0) {
		this->timer_delay--;
	}
	// Update sound timer
	if (this->timer_sound > 0) {
		// Beep at 0
		if (this->timer_sound == 1) {
			this->beepFlag = 1;
		}
		this->timer_sound--;
	}
}

// Executes the next opcode in memory
void CHIP8::executeOpcode() {
	int screen_iterator;
	int row_iterator;
	int cell_iterator;
	int row_offset;
	int cell_offset;
	uint8_t temporary_result;
	int addition_result;

	// Check first digit of opcode (35 possible opcodes)
	switch (this->opcode & 0xF000) {
		// 3 Possible opcodes (0x0___)
		case 0x0000:
			// Check last digit (0x0__?)
			switch (this->opcode & 0x000F) {
				// 0x00E0: Clears the screen
				case 0x0000:
					memset(this->display, 0, GRAPHICS_SIZE * sizeof(this->display[0]));
					this->drawFlag = 1;
					this->program_counter += 2;
				break;
				// 0x00EE: Returns from subroutine
				case 0x000E:
					// Get stored return address and increment pc, decrement stack pointer from escaped frame
					this->program_counter = this->stack[--(this->stack_pointer)] + 2;
				break;
				// 0x0000 Unimplemented
				default:;
			}
		break;
		// 0x1NNN Jumps to address NNN
		case 0x1000:
			this->program_counter = this->opcode & 0x0FFF;
		break;
		// 0x2NNN Calls subroutine at NNN
		case 0x2000:
			// Store address in stack
			this->stack[(this->stack_pointer)++] = this->program_counter;
			// Execute subroutine at 0x_NNN
			this->program_counter = this->opcode & 0x0FFF;
		break;
		// 0x3XNN Skips the next instruction if VX equals NN
		case 0x3000:
			this->program_counter += (this->opcode & 0x00FF) == this->registers[(this->opcode & 0x0F00) >> 8] ? 4 : 2;
		break;
		// 0x4XNN Skips the next instruction if VX doesnt equal NN
		case 0x4000:
			this->program_counter += (this->opcode & 0x00FF) != this->registers[(this->opcode & 0x0F00) >> 8] ? 4 : 2;
		break;
		// 0x5XY0 Skips the next instruction if VX equals VY
		case 0x5000:
			this->program_counter += this->registers[(this->opcode & 0x0F00) >> 8] == this->registers[(this->opcode & 0x00F0) >> 4] ? 4 : 2;
		break;
		// 0x6XNN Sets VX to NN
		case 0x6000:
			this->registers[(this->opcode & 0X0F00) >> 8] = this->opcode & 0x00FF;
			this->program_counter += 2;
		break;
		// 0x7XNN Adds NN to VX
		case 0x7000:
			this->registers[(this->opcode & 0x0F00) >> 8] += this->opcode & 0x00FF;
			this->program_counter += 2;
		break;
		// 9 possible opcodes (0x8___)
		case 0x8000:
			// Check last digit (0x8__?)
			switch(this->opcode & 0x000F) {
				// 0x8XY0 Sets VX to the value of VY
				case 0x0000:
					this->registers[(this->opcode & 0x0F00) >> 8] = this->registers[(this->opcode & 0x00F0) >> 4];
					this->program_counter += 2;
				break;
				// 0x8XY1 Sets VX to VX or VY
				case 0x0001:
					this->registers[(this->opcode & 0x0F00) >> 8] |= this->registers[(this->opcode & 0x00F0) >> 4];
					this->program_counter += 2;
				break;
				// 0x8XY2 Sets VX to VX and VY
				case 0x0002:
					this->registers[(this->opcode & 0x0F00) >> 8] &= this->registers[(this->opcode & 0x00F0) >> 4];
					this->program_counter += 2;
				break;
				// 0x8XY3 Sets VX to VX xor VY
				case 0x0003:
					this->registers[(this->opcode & 0x0F00) >> 8] ^= this->registers[(this->opcode & 0x00F0) >> 4];
					this->program_counter += 2;
				break;
				// 0x8XY4 Adds VY to VX. VF is set to 1 when there is a carry and 0 when there isn't
				case 0x0004:
					temporary_result = this->registers[(this->opcode & 0x0F00) >> 8] + this->registers[(this->opcode & 0x00F0) >> 4];
					this->registers[0xF] = this->registers[(this->opcode & 0x0F00) >> 8] > temporary_result ? 1 : 0; // set carry
					this->registers[(this->opcode & 0x0F00) >> 8] = temporary_result;
					this->program_counter += 2;
				break;
				// 0x8XY5 VY is subtracted from VX. VF is set to 0 when there is a borrow, and 1 when there isn't
				case 0x0005:
					// set carry
					if (this->registers[(this->opcode & 0x00F0) >> 4] > this->registers[(this->opcode & 0x0F00) >> 8]) {
						this->registers[0xF] = 0;
					} else {
						this->registers[0xF] = 1;
					}
					this->registers[(this->opcode & 0x0F00) >> 8] -= this->registers[(this->opcode & 0x00F0) >> 4];
					this->program_counter += 2;
				break;
				// 0x8XY6 Shifts VX right by 1. VF is set to the value of the least significant bit of VX before the shift
				case 0x0006:
					this->registers[0XF] = this->registers[(this->opcode & 0x0F00) >> 8] & 0x1;
					this->registers[(this->opcode & 0x0F00) >> 8] >>= 1;
					this->program_counter += 2;
				break;
				// 0x8XY7 Sets VX to VY minus VX. VF is set to 0 when there is a borrow, and 1 when there isn't
				case 0x0007:
					// set carry
					if (this->registers[(this->opcode & 0x0F00) >> 8] > this->registers[(this->opcode & 0x00F0) >> 4]) {
						this->registers[0xF] = 0;
					} else {
						this->registers[0xF] = 1;
					}
					this->registers[(this->opcode & 0x0F00) >> 8] = this->registers[(this->opcode & 0x00F0) >> 4] - this->registers[(this->opcode & 0x0F00) >> 8];
					this->program_counter += 2;
				break;
				// 0x8XYE Shifts VX to the left by 1. VF is set to the value of the most significant bit of VX before the shift
				case 0x000E:
					this->registers[0xF] = this->registers[(this->opcode & 0x0F00) >> 8] >> 7;
					this->registers[(this->opcode & 0x0F00) >> 8] <<= 1;
					this->program_counter += 2;
				break;
				// 0x8000 Unimplemented
				default:;
			}
		break;
		// 0x9XY0 Skips the next instruction if VX doesn't equal VY
		case 0x9000:
			this->program_counter += this->registers[(this->opcode & 0x0F00) >> 8] != this->registers[(this->opcode & 0x00F0) >> 4] ? 4 : 2;
		break;
		// 0xANNN Sets I to the address NNN
		case 0xA000:
			this->index = (this->opcode & 0X0FFF);
			this->program_counter += 2;
		break;
		// 0xBNNN Jumps to the address NNN plus V0
		case 0xB000:
			this->program_counter = this->registers[0] + (this->opcode & 0x0FFF);
		break;
		// 0xCXNN Sets VX to the result fo a bitwise and operation on a random number and NN
		case 0xC000:
			this->registers[(this->opcode & 0x0F00) >> 8] = (this->opcode & 0x00FF) & (rand() % 0xFF);
			this->program_counter += 2;
		break;
		// 0xDXYN Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of 
		// N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value 
		// doesn’t change after the execution of this instruction. As described above, VF is set to 1 
		// if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that 
		// doesn’t happen
		case 0xD000:
			// Initially assume there is no flipped bit
			this->registers[0xF] = 0;
			// Hit each line in the sprite
			for (row_iterator = 0; row_iterator < (this->opcode & 0x000F); row_iterator++) {
				// Hit each pixel in the line
				for (cell_iterator = 0; cell_iterator < 8; cell_iterator++) {
					// If the pixel is not set continue to the next pixel
					if ((this->memory[this->index + row_iterator] & (0x80 >> cell_iterator)) == 0) {
						continue;
					}
					row_offset = (this->registers[(this->opcode & 0x00F0) >> 4] + row_iterator) * GRAPHICS_WIDTH;
					cell_offset = row_offset + this->registers[(this->opcode & 0x0F00) >> 8] + cell_iterator;
					// Flip the pixel in the display
					this->display[cell_offset] ^= 1;
					// Check if the flipped flag needs to be set
					if (this->display[cell_offset] == 1) {
						this->registers[0xF] = 1;
					}
				}
			}
			this->drawFlag = 1;
			this->program_counter += 2;
		break;
		// 2 possible opcodes (0xEX__)
		case 0xE000:
			// Check the last two digets (0xEX??)
			switch (this->opcode & 0x00FF) {
				// 0xEX9E Skips the next instruction if the key stored in VX is pressed
				case 0x009E:
					this->program_counter += this->keypad[this->registers[(this->opcode & 0x0F00) >> 8]] ? 4 : 2;
				break;
				// 0xEX9E Skips the next instruction if the key stored in VX is not pressed
				case 0x00A1:
					this->program_counter += this->keypad[this->registers[(this->opcode & 0x0F00) >> 8]] ? 2 : 4;
				break;
				// 0xE000 Unimplemented
				default:;
			}
		break;
		// 9 Possible opcodes (0xFX__)
		case 0xF000:
			// Check the last two digets (0xFX??)
			switch (this->opcode & 0x00FF) {
				// 0xFX07 Sets VX to the value of the delay timer
				case 0x0007:
					this->registers[(this->opcode & 0x0F00) >> 8] = this->timer_delay;
					this->program_counter += 2;
				break;
				// 0xFX0A A key press is awated and stored in VX
				case 0x000A:
					temporary_result = 0;
					// Check each key if it is active
					for (cell_iterator = 0; cell_iterator < KEYPAD_SIZE; cell_iterator++) {
						// Store the first active key we see in the result register
						if (this->keypad[cell_iterator]) {
							temporary_result = 1;
							this->registers[(this->opcode & 0x0F00) >> 8] = cell_iterator;
						}
					}
					// Repeat instruction if no input is received
					if (! temporary_result) {
						break;
					}
					this->program_counter += 2;
				break;
				// 0xFX15 Sets the delay timer to VX
				case 0x0015:
					this->timer_delay = this->registers[(this->opcode & 0x0F00) >> 8];
					this->program_counter += 2;
				break;
				// 0xFX18 Sets the sound timer to VX
				case 0x0018:
					this->timer_sound = this->registers[(this->opcode & 0x0F00) >> 8];
					this->program_counter += 2;
				break;
				// 0xFX1E adds VX to I VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when
				// there isn't. This is undocumented feature of the CHIP-8 and used by Spacefight 2091! game.
				case 0x001E:
					addition_result = this->index + this->registers[(this->opcode & 0x0F00) >> 8];
					// Check for overflow
					this->registers[0xF] = addition_result > 0xFFF ? 1 : 0;
					this->index = addition_result % 0xFFF;
					this->program_counter += 2;
				break;
				// 0xFX29 Sets I to the location of the sprite for the character in VX.
				// Characters in 0-F (in hexedecimal) are represented by a 4x5 font
				case 0x0029:
					this->index = 0x5 * this->registers[(this->opcode & 0x0F00) >> 8];
					this->program_counter += 2;
				break;
				// 0xFX33 Stores the binary-coded decimal representation of VX, with the most significant of the
				// three digits at the address I, the midle digit at I+1, and the least significant digit at I+2
				// (In other words, take the decimal representation of VX, place the hundreds digit in memory at
				// location in I, the tens digit at location I+1, and the ones digit at location I+2.)
				case 0x0033:
					temporary_result = this->registers[(this->opcode & 0x0F00) >> 8];
					this->memory[this->index] = temporary_result / 100;
					this->memory[this->index + 1] = (temporary_result % 100) / 10;
					this->memory[this->index + 2] = temporary_result % 10;
					this->program_counter += 2;
				break;
				// 0xFX55 Stores V0 to VX (including VX) in memory starting at address I
				case 0x0055:
					temporary_result = (this->opcode & 0x0F00) >> 8;
					// Iterate over the registers
					for (cell_iterator = 0; cell_iterator < temporary_result; cell_iterator++) {
						this->memory[this->index + cell_iterator] = this->registers[cell_iterator];
					}
					// Incerase the index
					this->index += temporary_result + 1;
					this->program_counter += 2;
				break;
				// 0xFX65 Fills V0 to VX (including VX) with values from memory starting from address I
				case 0x0065:
					temporary_result = (this->opcode & 0x0F00) >> 8;
					// Iterate over the registers
					for (cell_iterator = 0; cell_iterator < temporary_result; cell_iterator++) {
						this->registers[cell_iterator] = this->memory[this->index + cell_iterator];
					}
					// Incerase the index
					this->index += temporary_result + 1;
					this->program_counter += 2;
				break;
				// 0xFX00 not implemented
				default:;
			}
		break;
		// All Opcodes implemented
		default:;
	}

}

void CHIP8::reset() {
	int memory_iterator;
	int register_iterator;
	int key_iterator;
	int display_iterator;
	int stack_iterator;
	int fontset_iterator;

	srand(time(NULL));

	this->timer_delay = 0;
	this->timer_sound = 0;

	this->program_counter = PROGRAM_START;
	this->opcode = 0;
	this->index = 0;
	this->stack_pointer = 0;

	// Set memory to empty
	memset(this->memory, 0, MEMORY_SIZE * sizeof(this->memory[0]));
	// Set registers to empty
	memset(this->registers, 0, NUM_REGISTERS * sizeof(this->registers[0]));
	// Clear keypresses
	memset(this->keypad, 0, KEYPAD_SIZE * sizeof(this->keypad[0]));
	// Clear the display
	memset(this->display, 0, GRAPHICS_SIZE * sizeof(this->display[0]));
	// Clear the stack
	memset(this->stack, 0, STACK_SIZE * sizeof(this->stack[0]));

	//Load the fontset into memory
	for (fontset_iterator = 0; fontset_iterator < CHIP8_FONTSET_SIZE; fontset_iterator++) {
		this->memory[fontset_iterator] = chip8_fontset[fontset_iterator];
	}

	// Draw the blank screen
	this->drawFlag = 1;
}