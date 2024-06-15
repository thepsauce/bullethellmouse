#include "entity.h"
#include "window.h"
#include "list.h"
#include "collision.h"

// Types of entity:
// 1. Projectile
// - Moves along a curve that goes from source to destination
// 2. Spawner
// - Spawns projectiles in a special pattern
// 3. Mover
// - Can simply move, the player is also of this class
// What they have in common:
// 1. Hit-/Hurtbox
// 2. Hit-/Hurtmask
// 3. Position, Radius
// 4. Health points, Damage points

struct entity *Entities[ENTITY_TYPE_MAX];
uint32_t Locks[ENTITY_TYPE_MAX];

struct template **templates;

__attribute__((constructor)) void init(void)
{
	for (uint32_t t = 0; t < ENTITY_TYPE_MAX; t++) {
		gsnew(Entities[t], 8);
	}
	lsnew(templates, 20);
}

__attribute__((destructor)) void uninit(void)
{
	for (uint32_t t = 0; t < ENTITY_TYPE_MAX; t++) {
		gsdelete(Entities[t]);
	}
	lsdelete(templates);
}

void add_template(const struct template *template)
{
	lsinsert(templates, UINT32_MAX, &template);
}

Entity create_entity_from_template(const char *name)
{
	lsforeach(templates, t) {
		if (strcmp(t->name, name) == 0) {
			return create_entity(t->type, &t->base);
		}
	}
	ASSERT(0, "template doesn't exist");
	return (Entity) { -1, -1 };
}

Entity create_entity(uint16_t type, const struct entity *entity)
{
	struct entity *es;
	struct entity *e;

	ASSERT(Locks[type] == 0, "Entities are locked");
	es = Entities[type];
	e = gsappend(es, entity);
	Entities[type] = es;
	return (Entity) { type, e - es };
}

int delete_entity(Entity entity)
{
	if (Locks[entity.type] > 0) {
		return 1;
	}
	Entities[entity.type][entity.index].shape = SHAPE_NONE;
	gsremove(Entities[entity.type], entity.index);
	return 0;
}

struct entity *lock_entity(Entity entity)
{
	Locks[entity.type]++;
	return Entities[entity.type] + entity.index;
}

void unlock_entity(Entity entity)
{
	Locks[entity.type]--;
}

static bool (*intersect_methods[SHAPE_MAX][SHAPE_MAX])(const struct entity *e1, const struct entity *e2, Vec2f *mtv) = {
	[SHAPE_CIRCLE] = {
		[SHAPE_CIRCLE] = intersect_circle_circle,
		[SHAPE_POLYGON] = intersect_circle_polygon,
	},
	[SHAPE_POLYGON] = {
		[SHAPE_CIRCLE] = intersect_polygon_circle,
		[SHAPE_POLYGON] = intersect_polygon_polygon,
	},
};

bool intersect_entity(Entity entity1, Entity entity2, Vec2f *mtv)
{
	struct entity *e1, *e2;
	bool intersect;

	e1 = lock_entity(entity1);
	e2 = lock_entity(entity2);

	intersect = intersect_methods[e1->shape][e2->shape](e1, e2, mtv);

	unlock_entity(entity1);
	unlock_entity(entity2);
	return intersect;
}

bool contains_entity(Entity entity, Vec2f point)
{
	struct entity *e;
	bool contained = false;
	Vec2f delta;

	e = lock_entity(entity);

	delta = vec_sub(e->pos, point);
	switch(e->shape) {
	case SHAPE_CIRCLE:
		contained = (uint32_t) vec_dot(delta, delta) < SQUARE(e->radius);
		break;
	default:
		ASSERT(0, "not implemented");
	}
	unlock_entity(entity);
	return contained;
}

static inline void update_projectiles(uint32_t ticks)
{
	// move projectiles along their designated path
	struct entity *es = Entities[ENTITY_TYPE_PROJECTILE];
	for (uint16_t i = 0; i < (uint16_t) gscount(es); i++) {
		uint32_t time;
		float t;
		struct entity *e = es + i;

		if (e->shape == SHAPE_NONE) {
			continue; // skip over gaps
		}
		time = e->curTime += ticks;
		if (time >= e->maxTime) {
			e->shape = SHAPE_NONE;
			gsremove(es, i);
			continue;
		}
		t = (float) e->curTime / (float) e->maxTime;
		switch(e->curve) {
		case ACURVE_LINE:
			e->pos.x = e->source.x + (e->dest.x - e->source.x) * t;
			e->pos.y = e->source.y + (e->dest.y - e->source.y) * t;
			break;
		}
	}
	Entities[ENTITY_TYPE_PROJECTILE] = es;
}

static inline void update_spawners(uint32_t ticks)
{
	struct entity *const es = Entities[ENTITY_TYPE_SPAWNER];
	for (uint16_t i = 0; i < (uint16_t) gscount(es); i++) {
		struct entity *e = es + i;
		if (e->shape == SHAPE_NONE) {
			continue;
		}
		e->tick(e, ticks);
	}
}

#define GRID_SIZE 25
#define CELL_SIZE 200
static Entity *Grid[GRID_SIZE][GRID_SIZE];

static Vec2i get_grid_offset(void)
{
	return (Vec2i) {
		(GRID_SIZE * CELL_SIZE - Window.size.x) / 2,
		(GRID_SIZE * CELL_SIZE - Window.size.y) / 2
	};
}

static inline void fill_grid(void)
{
	Vec2i gridOff = get_grid_offset();
	for (uint16_t t = 0; t < ENTITY_TYPE_MAX; t++) {
		struct entity *const es = Entities[t];
		for (uint16_t i = 0; i < (uint16_t) gscount(es); i++) {
			if (es[i].shape == SHAPE_NONE) {
				continue;
			}
			const Vec2f pos = es[i].pos;
			const Vec2i g = {
				((int) (pos.x + Window.scroll.x) + gridOff.x) / CELL_SIZE,
				((int) (pos.y + Window.scroll.y) + gridOff.y) / CELL_SIZE
			};
			if (g.x < 0 || g.y < 0 || g.x >= GRID_SIZE || g.y >= GRID_SIZE) {
				continue;
			}
			lsinsert(Grid[g.x][g.y], UINT32_MAX, (&(Entity) { t, i }));
		}
	}
}

static inline void compare_entities(struct entity *e1, struct entity *e2)
{
	if (e1->weight < e2->weight) {
		struct entity *tmp = e1;
		e1 = e2;
		e2 = tmp;
	}

	Vec2f mtv;
	if (intersect_methods[e1->shape][e2->shape](e1, e2, &mtv)) {
		e2->pos = vec_sub(e2->pos, mtv);
		if (e1->hitMask & e2->hurtMask) {
			e2->hp -= e1->dmg;
		}
		if (e1->hurtMask & e2->hitMask) {
			e1->hp -= e2->dmg;
		}
	}
}

static inline void compare_cells(Entity *c1, Entity *c2)
{
	lsforeach(c1, g1) {
		lsforeach(c2, g2) {
			compare_entities(Entities[g1.type] + g1.index, Entities[g2.type] + g2.index);
		}
	}
}

static inline void
check_grid_collision(void)
{
	// this compares each cell against it's nebhoring cell, but only once
	// this is how he travel:
	//	 #,
	//	,,,
	Vec2i gridOff = get_grid_offset();
	for (uint16_t col = 0; col < GRID_SIZE; col++) {
		for (uint16_t row = 0; row < GRID_SIZE; row++) {
			SDL_Rect r = { col * CELL_SIZE - gridOff.x, row * CELL_SIZE - gridOff.y, CELL_SIZE, CELL_SIZE };
			rectangleColor(Window.renderer, r.x, r.y, r.x + r.w, r.y + r.h, 0x22808000);
			char buf[30];
			snprintf(buf, sizeof(buf), "%u", lscount(Grid[col][row]));
			stringColor(Window.renderer, r.x + r.w / 2 - 10, r.y + r.h / 2, buf, 0xaaaaaaff);

			for (uint16_t i = 0; i < (uint16_t) lscount(Grid[col][row]); i++) {
				for (uint16_t j = i + 1; j < (uint16_t) lscount(Grid[col][row]); j++) {
					compare_entities(Entities[Grid[col][row][i].type] + Grid[col][row][i].index,
							Entities[Grid[col][row][j].type] + Grid[col][row][j].index);
				}
			}
			if (col < GRID_SIZE - 1) {
				if (row < GRID_SIZE - 1) {
					compare_cells(Grid[col][row], Grid[col + 1][row + 1]);
				}
				compare_cells(Grid[col][row], Grid[col + 1][row]);
			}
			if (row < GRID_SIZE - 1) {
				if (col) {
					compare_cells(Grid[col][row], Grid[col - 1][row + 1]);
				}
				compare_cells(Grid[col][row], Grid[col][row + 1]);
			}
		}
	}
}

static inline void render_entities(void)
{
	for (uint16_t t = 0; t < ENTITY_TYPE_MAX; t++) {
		for (uint16_t i = 0; i < (uint16_t) gscount(Entities[t]); i++) {
			struct entity *e = Entities[t] + i;
			if (e->shape == SHAPE_NONE) {
				continue;
			}
			if (e->hp < 0) {
				e->shape = SHAPE_NONE;
				gsremove(Entities[t], i);
			} else {
				switch(e->shape) {
				case SHAPE_CIRCLE:
					//filledEllipseColor(Window.renderer, e->pos.x + Scroll.x, e->pos.y + Scroll.y,
				//			e->radius, e->radius, e->color);
					aaellipseColor(Window.renderer,
							e->pos.x + Window.scroll.x,
							e->pos.y + Window.scroll.y,
							e->radius, e->radius, e->color);
					break;
				case SHAPE_POLYGON: {
					int16_t vx[e->n];
					int16_t vy[e->n];
					int16_t x = e->pos.x + Window.scroll.x;
					int16_t y = e->pos.y + Window.scroll.y;
					for (uint16_t i = 0; i < e->n; i++) {
						vx[i] = e->vertices[i].x + x;
						vy[i] = e->vertices[i].y + y;
					}
					aapolygonColor(Window.renderer, vx, vy, e->n, e->color);
					break;
				}
				}
				char buf[30];
				snprintf(buf, sizeof(buf), "%u", e->hp);
				stringColor(Window.renderer,
						e->pos.x + Window.scroll.x,
						e->pos.y + Window.scroll.y,
						buf, 0xffffffff);
			}
		}
	}
}

void update_entities(uint32_t ticks)
{
	// initialize cells
	for (uint16_t col = 0; col < GRID_SIZE; col++) {
		for (uint16_t row = 0; row < GRID_SIZE; row++) {
			if (Grid[col][row] == NULL) {
				lsnew(Grid[col][row], 10);
			} else {
				lscount(Grid[col][row]) = 0;
			}
		}
	}
	update_projectiles(ticks);
	update_spawners(ticks);
	fill_grid();
	check_grid_collision();
	render_entities();
}

