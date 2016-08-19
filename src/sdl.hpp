#ifndef _H_SDL_IO
#define _H_SDL_IO

#include <iostream>
#include <inttypes.h>
#include <SDL2/SDL.h>

class SDLDisplay {

public:
	/********************
	* Create a new sdl display screen with the specified dimensions.
	* @param width  pixel width of the display screen
	* @param height pixel height of the display screen
	********************/
	SDLDisplay(int width, int height);

	/********************
	* Create a new sdl display screen with the specified dimensions and zoom.
	* Pixels can be set to a "zoom". This stretches the pixels.
	* This means, for example, if a screen was set to have a height
	* of 10 and width of 10 and "zoom" of 2, each virtual pixel will be
	* displayed as a 2*2 block of true pixels on the screen, creating a 
	* display which is accessed and functions as a 10*10 display, but 
	* uses is actually 20*20 true pixels wide.
	* @param width  virtual pixel width of the display screen
	* @param height virtual pixel height of the display screen
	* @param zoom   true screen pixel size of each virtual pixel
	*******************/
	SDLDisplay(int width, int height, int zoom);

	/*******************
	* Destructs an SDLDisplay instance
	*******************/
	~SDLDisplay();

	/*******************************
	* Changes the specified location to the specified value
	* @param row   Row in display grid to change
	* @param cell  Cell in row to change
	*******************************/
	void setPixel(int row, int cell, uint8_t red, uint8_t green, uint8_t blue);

	/*******************************
	* Gets the pixel value at the specified location
	* @param row   Row in display grid to change
	* @param cell  Cell in row to change
	*******************************/
	uint8_t getPixel(int row, int cell);

	/*******************************
	* Refreshes the display to reflect any changes
	*******************************/
	void refresh();

	/*******************************
	* Locks the SDL screen for display
	*******************************/
	void lockScreen();

	/*******************************
	* Unlocks the SDL screen for editing
	*******************************/
	void unlockScreen();

	/*******************
	* Check the keymap and set the results array to 1 at each pressed
	* key in the map.
	* @param keys    number of keys in keymap
	* @param keymap  keys to check if are active
	* @param results result array location to store pressed keys
	*******************/
	void setKeys(uint8_t keys, int * keymap, uint8_t * results);

	/*******************************
	* Sleep the sdl display thread
	* @param ms milliseconds to sleep for
	*******************************/
	void sleep(int ms);

private:
	/* Screen display should write to */
	SDL_Window * display_window;
	SDL_Renderer * display_renderer;
	SDL_Texture * display_texture;
	SDL_Surface * display_surface;

	/* Zoom level of the display */
	int zoom = 1;

	/* Height of the display */
	int height;
	int pixel_height;

	/* Width of the display */
	int width;
	int pixel_width;

	/* Flag for if display is locked */
	uint8_t locked;

	/*******************
	* Initialize the SDL systems
	*******************/
	void initialize();

	/*******************
	* Destroy the sdl display and free any allocated memory
	*******************/
	void destroy();
};

#endif