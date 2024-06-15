#include "window.h"
#include "entity.h"
#include "level.h"

#include <time.h>

int main(int argc, char **argv)
{
	SDL_Event event;
	bool running;
	Uint64 start, end;

	(void) argc;
	(void) argv;

	srand(time(NULL));

	init_window();

	start = SDL_GetTicks();

	// Run the event loop.
	create_entity_from_template("Player");
	for (int i = 0; i < 4; i++) {
		Entity e = create_entity_from_template("Random");
		struct entity *p = lock_entity(e);
		p->pos.x = (float) rand() * 640 / RAND_MAX;
		unlock_entity(e);
	}
	for (uint32_t i = 0; i < 1; i++) {
		const int r = rand() % 2;
		if (r) {
			create_entity(ENTITY_TYPE_MOVER, &(struct entity) {
				.pos = { 300, 285 },
				.color = 0xff885500,
				.shape = SHAPE_POLYGON,
				.n = 4,
				.vertices = {
					{ -20, -20 },
					{  20, -20 },
					{  20,  20 },
					{  -20,  20 },
				},
			});
		} else {
			create_entity(ENTITY_TYPE_MOVER, &(struct entity) {
				.pos = { 300, 285 },
				.color = 0xff885500,
				.shape = SHAPE_CIRCLE,
				.radius = 100,
			});
		}
	}
	SDL_SetWindowGrab(Window.sdl, true);
	running = true;
	while (running) {
		uint32_t ticks;

		SDL_SetRenderDrawColor(Window.renderer, 0, 0, 0, 0);
		SDL_RenderClear(Window.renderer);
		end = SDL_GetTicks();
		ticks = end - start;
		start = end;
		//if (SDL_GetWindowGrab(window))
		//Scroll.y += 1e-1f * ticks;
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					SDL_SetWindowGrab(Window.sdl, !SDL_GetWindowGrab(Window.sdl));
				}
				break;
			}
		}
		if (SDL_GetWindowGrab(Window.sdl)) {
			update_entities(ticks);
			update_level(ticks);
		} else {
			update_level_menu(ticks);
		}
		SDL_RenderPresent(Window.renderer);
	}
	// Clean up and exit.
	SDL_DestroyRenderer(Window.renderer);
	SDL_DestroyWindow(Window.sdl);
	SDL_Quit();
	return 0;
}

