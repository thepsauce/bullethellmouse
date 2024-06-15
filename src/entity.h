#ifndef ENTITY_H
#define ENTITY_H

#include "vec.h"

#include <stdbool.h>

enum {
	ENTITY_TYPE_PROJECTILE,
	ENTITY_TYPE_SPAWNER,
	ENTITY_TYPE_MOVER,
	ENTITY_TYPE_MAX,
};

typedef struct {
	uint16_t type;
	uint16_t index;
} Entity;

enum {
	ACURVE_NULL,
	ACURVE_NONE,
	ACURVE_LINE,
	ACURVE_CIRCLE,
};

enum {
	SHAPE_NONE, // entities with this shape are dead or deactivated
	SHAPE_CIRCLE,
	SHAPE_POLYGON,
	SHAPE_MAX,
};

struct entity {
	Vec2f pos;
	Vec2f min, max;
	uint32_t color; // TODO: Replace with texture
	uint16_t shape;
	uint16_t weight;
	union {
		struct { // polygon
			uint16_t n;
			sVec2i vertices[4];
		};
		struct { // circle
			uint16_t radius;
		};
	};
	uint16_t hitMask, hurtMask;
	int16_t hp, dmg;
	// special data of each entity
	union {
		struct { // projectile
			uint32_t curve;
			Vec2f source, dest;
			uint32_t curTime, maxTime;
		};
		struct { // spawner
			void (*tick)(struct entity *entity, uint32_t ticks);
			uint32_t accum;
		};
		struct { //  Mover
		};
	};
};

struct template {
	const char *name;
	uint16_t type;
	struct entity base;
};

void add_template(const struct template *template);
Entity create_entity_from_template(const char *name);
Entity create_entity(uint16_t type, const struct entity *entity);
int delete_entity(Entity entity);
struct entity *lock_entity(Entity entity);
void unlock_entity(Entity entity);
bool intersect_entity(Entity entity1, Entity entity2, Vec2f *mtv);
bool contains_entity(Entity entity, Vec2f point);
void update_entities(uint32_t ticks);

#endif
