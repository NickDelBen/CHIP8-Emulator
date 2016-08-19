#include "sdl.hpp"

// Create a new sdl display screen with the specified dimensions.
SDLDisplay::SDLDisplay(int width, int height) {
	this->width = width;
	this->height = height;
	this->initialize();
}

// Create a new sdl display screen with the specified dimensions and zoom.
SDLDisplay::SDLDisplay(int width, int height, int zoom) {
	this->zoom = zoom;
	this->width = width;
	this->height = height;
	this->initialize();
}


// Destructs an SDLDisplay instance
SDLDisplay::~SDLDisplay() {
	this->destroy();
}

// Destroys the sdl window
void SDLDisplay::destroy() {
	// Destroy the sdl texture
	if (this->display_texture != nullptr) {
		SDL_DestroyTexture(this->display_texture);
	}
	// Destroy the sdl renderer
	if (this->display_renderer != nullptr) {
		SDL_DestroyRenderer(this->display_renderer);
	}
	// Destroy the sdl window
	if (this->display_window != nullptr) {
		SDL_DestroyWindow(this->display_window);
	}
	// Quit the sdl framework
	SDL_Quit();
}

// Refreshes the display to reflect any changes
void SDLDisplay::refresh() {
	this->unlockScreen();
	// Create a new texture from the display surface
	this->display_texture = SDL_CreateTextureFromSurface(this->display_renderer, this->display_surface);
	// Display the texture in the window
	SDL_RenderCopy(this->display_renderer, this->display_texture, NULL, NULL);
	SDL_RenderPresent(this->display_renderer);
	this->lockScreen();
}

// Changes the specified location to the specifeid value
void SDLDisplay::setPixel(int row, int cell, uint8_t red, uint8_t green, uint8_t blue) {
	uint32_t* first_location;
	uint32_t* pixel_location;
	uint32_t pixel_color;
	int row_iterator;
	int cell_iterator;

	pixel_color = SDL_MapRGB(this->display_surface->format, red, green, blue);

	first_location = (uint32_t *) this->display_surface->pixels + ((row * this->pixel_width + cell) * this->zoom);
	for (row_iterator = 0; row_iterator < this->zoom; row_iterator++) {
		for (cell_iterator = 0; cell_iterator < this->zoom; cell_iterator++) {
			pixel_location = first_location + (row_iterator * this->pixel_width) + cell_iterator;
			*pixel_location = pixel_color;
		}
	}
}

// Gets the pixel value at the specified location
uint8_t SDLDisplay::getPixel(int row, int cell) {
	uint8_t* pixel_location;

	pixel_location = (uint8_t *) this->display_surface->pixels + (((row * this->width * this->zoom) + cell) * this->zoom);
	return *pixel_location;
}

// Initialize the SDL systems
void SDLDisplay::initialize() {
	//Compute the pixel dimensions
	this->pixel_width = this->width * this->zoom;
	this->pixel_height = this->height * this->zoom;
	// Initilize sdl framework
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		throw 1;
	}
	// Create the display window
	this->display_window = SDL_CreateWindow("Display_window_editme", 100, 100, this->pixel_width, this->pixel_height, SDL_WINDOW_SHOWN);
	if (this->display_window == nullptr){
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		throw 2;
	}
	// Create the display renderer
	this->display_renderer = SDL_CreateRenderer(this->display_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (this->display_renderer == nullptr){
		SDL_DestroyWindow(this->display_window);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		throw 3;
	}
	// Crete the display surface
	this->display_surface = SDL_CreateRGBSurface(0, this->pixel_width, this->pixel_height, 32, 0, 0, 0, 0);
	if(this->display_surface == nullptr) {
		SDL_DestroyRenderer(this->display_renderer);
		SDL_DestroyWindow(this->display_window);
		std::cout << "SDL_CReateRGBSurface Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
        throw 4;
    }
}

// Check the keymap and set the results array to 1 at each pressed
void SDLDisplay::setKeys(uint8_t keys, int * keymap, uint8_t * results) {
	int keymap_iterator;
	SDL_Event e;
	uint8_t * keystates;

	while (SDL_PollEvent(&e)){
		if (e.type == SDL_KEYDOWN) {
			keystates = (uint8_t *) SDL_GetKeyboardState(NULL);
			// Check each key in the keymap
			for (keymap_iterator = 0; keymap_iterator < keys; keymap_iterator++) {
				// Set the state of the keymap
				results[keymap_iterator] = keystates[keymap[keymap_iterator]] ? 1 : 0;
			}
		}
	}
}

// Locks the SDL screen for display
void SDLDisplay::lockScreen() {
	if ((!(this->locked)) && SDL_MUSTLOCK(this->display_surface)) {
		SDL_LockSurface(this->display_surface);
	}
	this->locked = 1;
}

// Unlocks the SDL screen for editing
void SDLDisplay::unlockScreen() {
	if (this->locked && SDL_MUSTLOCK(this->display_surface)) {
		SDL_UnlockSurface(this->display_surface);
	}
	this->locked = 0;
}

// Sleep the sdl display thread
void SDLDisplay::sleep(int ms) {
	SDL_Delay(ms);
}
