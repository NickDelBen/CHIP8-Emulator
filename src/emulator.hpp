#ifndef _H_CHIP8_EMULATOR
#define _H_CHIP8_EMULATOR

#include <SDL2/SDL.h>
#include <iostream>

#include "sdl.hpp"
#include "chip8.hpp"

//ms between clock (1000ms / 60hz ~= 12ms between refreshes)
#define CLOCK_DELAY 0

class CHIP8Emulator {
public:
	CHIP8Emulator() {};
	~CHIP8Emulator() {};

	/**********************
	* Loads a game and starts running it
	* @param file_path Path to the file containing program to load
	**********************/
	void startProgram(std::string file_path);

private:
	/* The display and input module */
	SDLDisplay display = SDLDisplay(GRAPHICS_WIDTH, GRAPHICS_HEIGHT, 8);

	/* Hardware */
	CHIP8 hardware = CHIP8();

	/* Default keymap */
	int keymap[KEYPAD_SIZE] = {
		SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
		SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
		SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
		SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V
	};

	/*******************
	* Check the keymap and set the results array to 1 at each pressed key in the map.
	*******************/
	void setKeys();

	/*******************
	* Draw the screen from the chips video memory to the screen
	*******************/
	void drawScreen();

	/*******************
	* Play the beep from the speakers
	*******************/
	void playBeep();

};

#endif
