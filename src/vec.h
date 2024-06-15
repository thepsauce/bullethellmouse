#ifndef VEC_H
#define VEC_H

#include "macros.h"

#include <stdint.h>
#include <math.h>

typedef struct {
	float min, max;
} fminmax_t;

typedef struct vector2i {
	int32_t x, y;
} Vec2i;

typedef struct small_vector2i {
	int16_t x, y;
} sVec2i;

typedef struct vector2f {
	float x, y;
} Vec2f;

#define vec_neg(a) ({ \
	__auto_type _a = (a); \
	(typeof(_a)) { -_a.x, -_a.y }; \
})

#define vec_add(a, b) ({ \
	__auto_type _a = (a); \
	__auto_type _b = (b); \
	(typeof(_a)) { _a.x + _b.x, _a.y + _b.y }; \
})

#define vec_sub(a, b) ({ \
	__auto_type _a = (a); \
	__auto_type _b = (b); \
	(typeof(_a)) { _a.x - _b.x, _a.y - _b.y }; \
})

#define vec_dot(a, b) ({ \
	__auto_type _a = (a); \
	__auto_type _b = (b); \
	_Generic(_a, \
		sVec2i: (int32_t) _a.x * (int32_t) _b.x + (int32_t) _a.y * (int32_t) _b.y, \
		Vec2i: (int64_t) _a.x * (int64_t) _b.x + (int64_t) _a.y * (int64_t) _b.y, \
		default: _a.x * _b.x + _a.y * _b.y); \
})

#define vec_scale(a, f) ({ \
	__auto_type _a = (a); \
	float _f = (f); \
	(typeof(_a)) { (typeof(_a.x)) ((float) _a.x * _f), (typeof(_a.y)) ((float) _a.y * _f) }; \
})

#define vec_normalize(a) ({ \
	__auto_type _a = (a); \
	ASSERT(_Generic(_a, \
		Vec2f: 1, \
		default: 0), "bad type for normalize"); \
	const float invSqrt = 1.0f / sqrt(_a.x * _a.x + _a.y * _a.y); \
	(Vec2f) { _a.x * invSqrt, _a.y * invSqrt }; \
})

#define vec_dist_sq(a, b) ({ \
	__auto_type _a = (a); \
	__auto_type _b = (b); \
	typeof(_a) _delta = { _a.x - _b.x, _a.y - _b.y }; \
	_Generic(_delta, \
		sVec2i: (int32_t) _delta.x * (int32_t) _delta.x + (int32_t) _delta.y * (int32_t) _delta.y, \
		Vec2i: (int64_t) _delta.x * (int64_t) _delta.x + (int64_t) _delta.y * (int64_t) _delta.y, \
		default: _delta.x * _delta.x + _delta.y * _delta.y); \
})

#endif
