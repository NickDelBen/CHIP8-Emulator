#include <iostream>

#include "emulator.hpp"

int main(int argc, char* argv[]) {
	// Check to ensure a program to run has been passed in
	if (argc != 2) {
		std::cout << "Proper Usage:\n    chip8 <path_to_program>" << std::endl;
		return 0;
	}
	// Create a new emulator
	CHIP8Emulator ce = CHIP8Emulator();
	// Load the specified program into the emulator
	ce.startProgram(argv[1]);
	return 0;
}

