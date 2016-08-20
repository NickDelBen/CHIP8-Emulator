
#include "emulator.hpp"

// Loads a game and starts running it
void CHIP8Emulator::startProgram(std::string file_path) {
	// Reset the hardware
	this->hardware.reset();
	// Load the program into the CHIP8 memory
	this->hardware.loadProgram(file_path);

	while (1) {
		// sleep between 
		this->display.sleep(CLOCK_DELAY);
		// Check if the display needs to be updated
		if (this->hardware.drawFlag) {
			this->drawScreen();
			this->hardware.drawFlag = 0;
		}
		// Check if the buzzer needs to be sounded
		if (this->hardware.beepFlag) {
			this->playBeep();
			this->hardware.beepFlag = 0;
		}
		// Check for any inputs
		this->setKeys();
		// Send a clock to the processor
		this->hardware.cycle();
	}
}

// Check the keymap and set the results array to 1 at each pressed key in the map.
void CHIP8Emulator::setKeys() {
	this->display.setKeys(KEYPAD_SIZE, this->keymap, this->hardware.keypad);
}

// Play the beep from the speakers
void CHIP8Emulator::playBeep() {
	//std::cout << "BUZZ BUZZ BUZZ" << std::endl;
	std::cout << '\a' << std::flush;
}

// Draw the screen from the chips video memory to the screen
void CHIP8Emulator::drawScreen() {
	int row_iterator;
	int cell_iterator;

	// Set all the pixels to their new values
	for (row_iterator = 0; row_iterator < GRAPHICS_HEIGHT; row_iterator++) {
		for (cell_iterator = 0; cell_iterator < GRAPHICS_WIDTH; cell_iterator++) {
			// Check if this cell is activated in the chips video memory
			if (this->hardware.display[(row_iterator * GRAPHICS_WIDTH) + cell_iterator]) {
				this->display.setPixel(row_iterator, cell_iterator, 255, 255, 255);
			} else {
				this->display.setPixel(row_iterator, cell_iterator, 0, 0, 0);
			}
		}
	}

	this->display.refresh();
}