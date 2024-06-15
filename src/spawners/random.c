#include "entity.h"

#include <string.h>
#include <stdlib.h>

static void random_tick(struct entity *entity, uint32_t ticks)
{
	entity->accum += ticks;
	if (entity->accum > 100) {
		const Vec2f source = entity->pos;
		const Vec2f dest = { (float) rand() * 800.0f / RAND_MAX, 500.0f };
		sVec2i vertices[] = {
			{ -12,   0 },
			{   0, -12 },
			{  12,   0 },
			{   0,  19 },
		};
		struct entity e;
		// rotate polygon towards destination
		for (uint16_t i = 0; i < 4; i++) {
			const float x = (float) vertices[i].x;
			const float y = (float) vertices[i].y;
			const float angle = -M_PI / 2 + atan2f(dest.y - source.y, dest.x - source.x);
			const float cos_a = cosf(angle);
			const float sin_a = sinf(angle);
			const float new_x = x * cos_a - y * sin_a;
			const float new_y = y * cos_a + x * sin_a;
			vertices[i] = (sVec2i) { (int16_t) new_x, (int16_t) new_y };
		}
		e = (struct entity) {
			.pos = source,
			.color = 0xffff00ff,
			.shape = SHAPE_POLYGON,
			.n = 4,
			// .vertices = vertices,
			.hitMask = 1,
			.dmg = 1,
			.hp = 12,

			.curve = ACURVE_LINE,
			.source = source,
			.dest = dest,
			.curTime = 0,
			.maxTime = 1000,
		};
		memcpy(e.vertices, vertices, sizeof(vertices));
		create_entity(ENTITY_TYPE_PROJECTILE, &e);
		entity->accum -= 100;
	}
}

static const struct template template = {
	.name = "Random",
	.type = ENTITY_TYPE_SPAWNER,
	.base = {
		.pos = { 320.0f, 0.0f },
		.color = 0xff119911,
		.tick = random_tick,
		.shape = SHAPE_CIRCLE,
		.radius = 10,
		.hp = 100,
		.hurtMask = 2,
	},
};

__attribute__((constructor)) static void
init(void)
{
	add_template(&template);
}
