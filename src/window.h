#ifndef WINDOW_H
#define WINDOW_H

#include "vec.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

extern struct window {
	SDL_Window *sdl;
	SDL_Renderer *renderer;
	const Uint8 *keys;
	Vec2f scroll;
	Vec2i size;
} Window;

int init_window(void);

#endif
