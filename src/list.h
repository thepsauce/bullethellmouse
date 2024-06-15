#ifndef INCLUDED_LIST_H
#define INCLUDED_LIST_H

#include "macros.h"

#include <stdlib.h>
#include <stdint.h>

struct list {
	uint32_t n, cap;
	char data[0];
};

#define lsnew(ls, init_cap) ({ \
	const size_t _sz = sizeof(*ls); \
	const uint32_t _cap = (init_cap); \
	struct list *_l = malloc(sizeof(*_l) + _cap * _sz); \
	ASSERT(_l, "out of memory"); \
	_l->n = 0; \
	_l->cap = _cap; \
	ls = (void*) (_l + 1); \
	ls; \
})

#define lsdelete(ls) free((struct list*) ls - 1)

#define lsinsert(ls, index, elem) ({ \
	const uint32_t _sz = sizeof(*ls); \
	const void *_e = (elem); \
	struct list *_l = (struct list*) ls - 1; \
	const uint32_t _i = (index) == UINT32_MAX ? _l->n : (index); \
	ASSERT(_i <= _l->n, "index out of bounds"); \
	if(_l->n + 1 > _l->cap) { \
		_l->cap = 1 + _l->cap * 2; \
		_l = realloc(_l, sizeof(*_l) + _l->cap * _sz); \
		ASSERT(_l, "out of memory"); \
	} \
	void *_dest = _l->data + _sz * _l->n++; \
	if(_e == NULL) { \
		memset(_dest, 0, _sz); \
	} else { \
		memcpy(_dest, _e, _sz); \
	} \
	ls = (void*) (_l + 1); \
	_dest; \
})

#define lsremove(ls, index) ({ \
	const uint32_t _sz = sizeof(*ls); \
	const uint32_t _i = (index); \
	struct list *_l = (struct list*) ls - 1; \
	ASSERT(_i < _l->n, "index out of bounds"); \
	_l->n--; \
	memmove(_l->data + _sz * _i, _l->data + _sz * (_i + 1), _sz * (_l->n - _i)); \
})

#define lsclear(ls) ({ \
	struct list *_l = (struct list*) ls - 1; \
	_l->n = 0; \
})

#define lscount(ls) ((struct list*) ls - 1)->n
#define lscap(ls) ((struct list*) ls - 1)->cap

#define lsforeach(ls, name) \
	for(typeof(*ls) *_s = ls, *_e = ls + lscount(ls), name; \
		_s != _e && ({ name = *_s; 1; }); \
		_s++)
#define lsforeachremove(ls) ({ \
	lscount(ls)--; \
	_e--; \
	memmove(_s, _s + 1, sizeof(*_s) * (_e - _s)); \
	_s--; \
})

struct glist {
	uint32_t n, nGaps;
	size_t capBytes;
	char data[0]; // char data[n];
	// uint32_t gaps[nGaps];
};

#define gsnew(gs, init_cap) ({ \
	const size_t _capBytes = sizeof(*gs) * (init_cap); \
	struct glist *_l = malloc(sizeof(*_l) + _capBytes); \
	ASSERT(_l, "out of memory"); \
	_l->n = 0; \
	_l->nGaps = 0; \
	_l->capBytes = _capBytes; \
	gs = (void*) (_l + 1); \
})

#define gsdelete(gs) free((struct glist*) gs - 1)

#define gsappend(gs, elem) ({ \
	const uint32_t _sz = sizeof(*gs); \
	const void *_e = (elem); \
	struct glist *_l = (struct glist*) gs - 1; \
	void *_dest; \
	if(_l->nGaps > 0) { \
		_l->nGaps--; \
		_dest = _l->data + _sz * ((uint32_t*) (_l->data + _sz * _l->n))[_l->nGaps]; \
	} else { \
		if(_sz * (_l->n + 1) > _l->capBytes) { \
			_l->capBytes = _sz + _l->capBytes * 2; \
			_l = realloc(_l, sizeof(*_l) + _l->capBytes); \
			ASSERT(_l, "out of memory"); \
			gs = (void*) (_l + 1); \
		} \
		_dest = _l->data + _sz * _l->n++; \
	} \
	if(_e == NULL) { \
		memset(_dest, 0, _sz); \
	} else { \
		memcpy(_dest, _e, _sz); \
	} \
	_dest; \
})

#define gsremove(gs, index) ({ \
	const uint32_t _i = (index); \
	struct glist *_l = (struct glist*) gs - 1; \
	ASSERT(_i < _l->n, "index out of bounds"); \
	const uint32_t _sz = sizeof(*gs); \
	const size_t _off = _sz * _l->n + sizeof(uint32_t) * _l->nGaps; \
	if(_off + sizeof(uint32_t) > _l->capBytes) { \
		_l->capBytes = sizeof(uint32_t) + 2 * _l->capBytes; \
		_l = realloc(_l, sizeof(*_l) + _l->capBytes); \
		ASSERT(_l, "out of memory"); \
		gs = (void*) (_l + 1); \
	} \
	*(uint32_t*) (_l->data + _off) = _i; \
	++_l->nGaps; \
})

#define gsclear(gs) ({ \
	struct glist *_l = (struct glist*) gs - 1; \
	_l->nGaps = 0; \
	_l->n = 0; \
})

#define gscompact(gs) ({ \
	const size_t _sz = sizeof(*gs); \
	struct glist *_l = (struct glist*) gs - 1; \
	uint32_t _n, _nGaps; \
	uint32_t *_gaps; \
	uint32_t _i, _j; \
	_n = _l->n; \
	_nGaps _l->nGaps; \
	_gaps = _l->data + _l->n * _sz; \
	for(_i = 0; _i < _n && j < _nGaps; _i++) { \
		if(_i == gaps[_j]) { \
			_j++; \
		} else if(_j) { \
			memmove(_l->data + _sz * (_i - _j), _l->data + _sz * _i, _sz); \
		} \
	} \
	_l->nGaps = 0; \
	_l->n -= _j; \
})

#define gscount(gs) ((struct glist*) gs - 1)->n
#define gsgapcount(gs) ((struct glist*) gs - 1)->nGaps

#endif
