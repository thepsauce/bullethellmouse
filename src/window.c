#include "window.h"

#include <stdio.h>

struct window Window;

int init_window(void)
{
	Window.size = (Vec2i) { 640, 800 };
	// Initialize SDL.
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL could not be initialized: %s\n", SDL_GetError());
		return -1;
	}
	Window.keys = SDL_GetKeyboardState(NULL);

	// Create a window.
	Window.sdl = SDL_CreateWindow(
		"My SDL2 Window",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		Window.size.x,
		Window.size.y,
		SDL_WINDOW_SHOWN
	);
	if (Window.sdl == NULL) {
		fprintf(stderr, "SDL window could not be created: %s\n", SDL_GetError());
		return -1;
	}
	//SDL_ShowCursor(SDL_DISABLE);
	// Create a renderer.
	Window.renderer = SDL_CreateRenderer(Window.sdl, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (Window.renderer == NULL) {
		fprintf(stderr, "SDL renderer could not be created: %s\n", SDL_GetError());
		SDL_DestroyWindow(Window.sdl);
		return -1;
	}
	return 0;
}
