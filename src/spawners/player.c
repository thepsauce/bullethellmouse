#include "window.h"
#include "entity.h"

#include <string.h>

static void player_tick(struct entity *player, uint32_t ticks)
{
	if (SDL_GetWindowGrab(Window.sdl)) {
		Vec2i mouse;
		Vec2f source, dest;
		static const sVec2i vVertices[] = {
			{ -2, -40 },
			{ -2,  40 },
			{  2,  40 },
			{  2, -40 },
		};
		static const sVec2i hVertices[] = {
			{ -40, -2 },
			{ -40,  2 },
			{  40,  2 },
			{  40, -2 },
		};
		const sVec2i *vertices;
		struct entity entity;

		SDL_GetMouseState(&mouse.x, &mouse.y);
		const Vec2f dir = (Vec2f) {
			mouse.x - Window.scroll.x - player->pos.x,
			mouse.y - Window.scroll.y - player->pos.y
		};
		const float dist_sq = vec_dot(dir, dir);
		const float mouse_speed = (float) ticks * 0.7f;
		if (dist_sq < mouse_speed * mouse_speed) {
			player->pos = (Vec2f) { mouse.x + Window.scroll.x, mouse.y + Window.scroll.y };
		} else {
			const float mul = mouse_speed / sqrt(dist_sq);
			player->pos.x += dir.x * mul;
			player->pos.y += dir.y * mul;
		}
		if (Window.keys[SDL_SCANCODE_W]) {
			source = (Vec2f) {
				player->pos.x,
				player->pos.y - player->radius + vVertices[0].y
			};
			dest = source;
			dest.y -= 800;
			vertices = vVertices;
		} else if (Window.keys[SDL_SCANCODE_A]) {
			source = (Vec2f) {
				player->pos.x - player->radius + hVertices[0].x,
				player->pos.y,
			};
			dest = source;
			dest.x -= 800;
			vertices = hVertices;
		} else if (Window.keys[SDL_SCANCODE_S]) {
			source = (Vec2f) {
				player->pos.x,
				player->pos.y + player->radius + vVertices[2].y
			};
			dest = source;
			dest.y += 800;
			vertices = vVertices;
		} else if (Window.keys[SDL_SCANCODE_D]) {
			source = (Vec2f) {
				player->pos.x + player->radius + hVertices[2].x,
				player->pos.y
			};
			dest = source;
			dest.x += 800;
			vertices = hVertices;
		} else {
			return;
		}

		entity = (struct entity) {
			.pos = source,
			.color = 0xffaaaaaa,
			.shape = SHAPE_POLYGON,
			.n = 4,
			//.vertices = vertices,
			.hitMask = 2,
			.dmg = 1,

			.curve = ACURVE_LINE,
			.source = source,
			.dest = dest,
			.curTime = 0,
			.maxTime = 300,
		};
		memcpy(entity.vertices, vertices, sizeof(entity.vertices));
		create_entity(ENTITY_TYPE_PROJECTILE, &entity);
	}
}

static const struct template template = {
	.name = "Player",
	.type = ENTITY_TYPE_SPAWNER,
	.base = {
		.pos = { 0.0f, 0.0f },
		.color = 0xff008800,
		.shape = SHAPE_POLYGON, // SHAPE_CIRCLE,
		.weight = 1,
		.n = 4,
		.vertices = {
			{ -20.0f, -20.0f },
			{ -20.0f,  20.0f },
			{  20.0f,  20.0f },
			{  20.0f, -20.0f },
		},
		//.radius = 8,
		.hitMask = 0,
		.hurtMask = 1,
		.hp = 1000,
		.dmg = 0,
		.tick = player_tick,
	},
};

__attribute__((constructor)) static void init(void)
{
	add_template(&template);
}
