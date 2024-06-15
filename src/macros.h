#ifndef MACROS_H
#define MACROS_H

#define ASSERT(a, msg) do { if(!(a)) fprintf(stderr, "%s:%s:%d - %s\n", __FILE__, __FUNCTION__, __LINE__, (msg)); } while(0)

#define MAX(a, b) ({ \
	__auto_type _a = (a); \
	__auto_type _b = (b); \
	_a > _b ? _a : _b; \
})

#define MIN(a, b) ({ \
	__auto_type _a = (a); \
	__auto_type _b = (b); \
	_a < _b ? _a : _b; \
})

#define ARRLEN(a) (sizeof(a)/sizeof*(a))

#define SQUARE(a) ({ \
	__auto_type _a = (a); \
	_a * _a; \
})

#define fclampf(x, a, b) fmax((a), fmin((b), (x)))

#endif
