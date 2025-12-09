#ifndef STRUCTS_H
# define STRUCTS_H

#include <unistd.h>

typedef struct {
	const char *data;
	size_t len;
} StringView;

typedef struct {
	long long	x;
	long long	y;
	long long	z;
} Point;

typedef struct {
	int			u;
	int			v;
	long long	dist_sq;
} Edge;

#endif // STRUCTS_H
