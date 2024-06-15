#ifndef COLLISION_H
#define COLLISION_H

#include "entity.h"

bool intersect_line(Vec2f a, Vec2f b, Vec2f c, Vec2f d);
bool intersect_circle_circle(const struct entity *e1, const struct entity *e2, Vec2f *mtv);
bool intersect_circle_polygon(const struct entity *e1, const struct entity *e2, Vec2f *mtv);
bool intersect_polygon_circle(const struct entity *e1, const struct entity *e2, Vec2f *mtv);
bool intersect_polygon_polygon(const struct entity *e1, const struct entity *e2, Vec2f *mtv);

#endif
