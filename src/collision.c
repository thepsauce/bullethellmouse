#include "collision.h"
#include "window.h"

bool intersect_circle_circle(const struct entity *e1, const struct entity *e2, Vec2f *mtv)
{
	const Vec2f delta = vec_sub(e1->pos, e2->pos);
	const float d = vec_dot(delta, delta);
	const float r = e1->radius + e2->radius;
	const float overlap_sq = r * r - d;
	if (overlap_sq <= 0) {
		return false;
	}
	if (mtv) {
		*mtv = vec_scale(vec_normalize(delta), r - sqrtf(d));
	}
	return true;
}

bool intersect_circle_polygon(const struct entity *e1, const struct entity *e2, Vec2f *mtv)
{
	float minOverlap = FLT_MAX;
	Vec2f smallestAxis = { 0.0f, 0.0f };
	const Vec2f pos = {
		e2->pos.x - e1->pos.x,
		e2->pos.y - e1->pos.y,
	};
	const float radius_sq = e1->radius * e1->radius;
	for (uint16_t i = 0; i < e2->n; i++) {
		const Vec2f p1 = {
			(float) e2->vertices[i].x,
			(float) e2->vertices[i].y
		};
		const Vec2f p2 = {
			(float) e2->vertices[(i + 1) % e2->n].x,
			(float) e2->vertices[(i + 1) % e2->n].y
		};

		const Vec2f edge = vec_sub(p2, p1);
		const Vec2f rel = vec_sub(pos, p1);
		const float t = vec_dot(rel, edge) / vec_dot(edge, edge);
		const Vec2f proj = vec_add(p1, vec_scale(edge, fclampf(t, 0.0f, 1.0f)));
		const float dist_sq = vec_dist_sq(pos, proj);
		if (dist_sq < radius_sq) {
			if (!mtv) {
				return true;
			}
			const float overlap = e1->radius - sqrtf(dist_sq);
			if (overlap < minOverlap) {
				minOverlap = overlap;
				smallestAxis = vec_normalize(vec_sub(proj, pos));
			}
		}
	}
	if (minOverlap < FLT_MAX) {
		*mtv = vec_scale(smallestAxis, minOverlap);
		Vec2f rel = vec_sub(*mtv, pos);
		if (vec_dot(*mtv, rel) < 0.0f) {
			*mtv = vec_scale(*mtv, -1.0f);
		}
		Vec2f draw = vec_scale(smallestAxis, 20.0f);
		thickLineColor(Window.renderer, e1->pos.x, e1->pos.y,
				e1->pos.x + draw.x, e1->pos.y + draw.y, 3, 0xff00ff33);
		return true;
	}
	return false;
}

bool intersect_polygon_circle(const struct entity *e1, const struct entity *e2, Vec2f *mtv)
{
	const bool intersect = intersect_circle_polygon(e2, e1, mtv);
	if (intersect)
		*mtv = (Vec2f) { -mtv->x, -mtv->y };
	return intersect;
}

bool intersect_line(Vec2f a, Vec2f b, Vec2f c, Vec2f d)
{
	const Vec2f s1 = { b.x - a.x, b.y - a.y },
		 s2 = { d.x - c.x, d.y - c.y };
	const float denom = s1.x * s2.y - s2.x * s1.y;
	const float s = (s1.x * (a.y - c.y) - s1.y * (a.x - c.x)) / denom,
		t = (s2.x * (a.y - c.y) - s2.y * (a.x - c.x)) / denom;
	return s >= 0 && s <= 1 && t >= 0 && t <= 1;
}

fminmax_t project(const struct entity *e, Vec2f axis)
{
	fminmax_t mm;
	mm.min = vec_dot(axis, e->vertices[0]);
	mm.max = mm.min;
	for (uint16_t i = 1; i < e->n; i++) {
		float p = vec_dot(axis, e->vertices[i]);
		if (p < mm.min) {
			mm.min = p;
		}
		if (p > mm.max) {
			mm.max = p;
		}
	}
	return mm;
}

bool intersect_polygon_polygon(const struct entity *e1, const struct entity *e2, Vec2f *mtv)
{
	float minOverlap = FLT_MAX;
	Vec2f smallestAxis = { 0, 0 };
	bool me = false;
	const Vec2f delta = vec_sub(e1->pos, e2->pos);
	for (uint16_t i = 0; i < e1->n; i++) {
		const Vec2f vertex = (Vec2f) {
			(float) e1->vertices[i].x + e1->pos.x,
			(float) e1->vertices[i].y + e1->pos.y,
		};
		const Vec2f next_vertex = (Vec2f) {
			(float) e1->vertices[(i + 1) % e1->n].x + e1->pos.x,
			(float) e1->vertices[(i + 1) % e1->n].y + e1->pos.y,
		};
		const Vec2f edge = vec_sub(next_vertex, vertex);
		const Vec2f axis = vec_normalize(((Vec2f) { -edge.y, edge.x }));
		const fminmax_t mm1 = project(e1, axis);
		const fminmax_t mm2 = project(e2, axis);
		const float off = vec_dot(axis, delta);
		const float overlap = mm1.max - mm2.min + off;
		if (overlap <= 0) {
			return false;
		} else if (overlap < minOverlap) {
			minOverlap = overlap;
			smallestAxis = axis;
			me = true;
		}
	}
	for (uint16_t i = 0; i < e2->n; i++) {
		const Vec2f vertex = (Vec2f) {
			(float) e2->vertices[i].x + e2->pos.x,
			(float) e2->vertices[i].y + e2->pos.y,
		};
		const Vec2f next_vertex = (Vec2f) {
			(float) e2->vertices[(i + 1) % e2->n].x + e2->pos.x,
			(float) e2->vertices[(i + 1) % e2->n].y + e2->pos.y,
		};
		const Vec2f edge = vec_sub(next_vertex, vertex);
		const Vec2f axis = vec_normalize(((Vec2f) { -edge.y, edge.x }));
		const fminmax_t mm1 = project(e1, axis);
		const fminmax_t mm2 = project(e2, axis);
		const float off = vec_dot(axis, delta);
		const float overlap = mm1.max - mm2.min - off;
		if (overlap <= 0) {
			return false;
		} else if (overlap < minOverlap) {
			minOverlap = overlap;
			smallestAxis = axis;
			me = false;
		}
	}
	if (mtv) {
		*mtv = vec_scale(smallestAxis, me ? -minOverlap : minOverlap);
	}
	return true;
	/*float minOverlap = 1.0f/0.0f;
	Vec2f smallestAxis = { 0, 0 };
	const Vec2f delta = vec_sub(e1->pos, e2->pos);
	for (uint16_t i = 0; i < e1->n; i++) {
		const Vec2f vertex = (Vec2f) {
			(float) e1->vertices[i].x,
			(float) e1->vertices[i].y,
		};
		const Vec2f next_vertex = (Vec2f) {
			(float) e1->vertices[(i + 1) % e1->n].x,
			(float) e1->vertices[(i + 1) % e1->n].y,
		};
		const Vec2f edge = vec_sub(next_vertex, vertex);
		const Vec2f axis = vec_normalize(((Vec2f) { -edge.y, edge.x }));
		const fminmax_t mm1 = project(e1, axis);
		const fminmax_t mm2 = project(e2, axis);
		const float off = vec_dot(axis, delta);
		const float overlap = fminf(mm1.max + off, mm2.max) - fmaxf(mm1.min + off, mm2.min);
		if (overlap <= 0)
			return false;
		printf("overlap=%f\n", overlap);
		if (overlap < minOverlap) {
			minOverlap = overlap;
			smallestAxis = axis;
		}
	}
	if (mtv) {
		*mtv = smallestAxis;
	}
	printf("%f, %f\n", smallestAxis.x, smallestAxis.y);
	return true;*/
}

/*bool intersect_polygon_polygon(const struct entity *a, const struct entity *b)
{
	Vec2f mtv;
	bool in = intersect(a, b, &mtv);
	if (in) {
		printf("%f, %f\n", mtv.x, mtv.y);
	}
	return in;
	for (uint16_t i = 0; i < a->n; i++) {
		const Vec2f apos = { a->pos.x + a->vertices[i].x, a->pos.y + a->vertices[i].y };
		const uint16_t next_i = (i + 1) % a->n;
		const Vec2f next_apos = { a->pos.x + a->vertices[next_i].x, a->pos.y + a->vertices[next_i].y };
		for (uint16_t j = 0; j < b->n; j++) {
			const Vec2f bpos = { b->pos.x + b->vertices[j].x, b->pos.y + b->vertices[j].y };
			const uint16_t next_j = (j + 1) % b->n;
			const Vec2f next_bpos = { b->pos.x + b->vertices[next_j].x, b->pos.y + b->vertices[next_j].y };
			if (intersect_line(apos, next_apos, bpos, next_bpos)) {
				return true;
			}
		}
	}
	return false;
}*/

