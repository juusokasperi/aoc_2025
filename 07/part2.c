#define MEMARENA_IMPLEMENTATION
#include "memarena.h"
#include "structs.h"
#include <unistd.h>
#include <stdio.h>

void	quicksort_edges(Edge *arr, int low, int high);

StringView	sv_chop(StringView *sv, char delimiter)
{
	StringView	result = *sv;

	for (size_t i = 0; i < sv->len; ++i)
	{
		if (sv->data[i] == delimiter)
		{
			result.len = i;
			sv->data += i + 1;
			sv->len -= i + 1;
			return (result);
		}
	}
	sv->data += sv->len;
	sv->len = 0;
	return (result);
}

long long	sv_to_int(StringView sv)
{
	unsigned long long	result = 0;
	long long			sign = 1;
	size_t				i = 0;

	if (sv.len > 0 && sv.data[0] == '-')
	{
		sign = -1;
		i++;
	}

	while (i < sv.len)
	{
		if (sv.data[i] >= '0' && sv.data[i] <= '9')
			result = result * 10 + (sv.data[i] - '0');
		i++;
	}
	return (result * sign);
}

int	find_set(int *parent, int i)
{
	if (parent[i] == i)
		return (i);
	return (parent[i] = find_set(parent, parent[i]));
}

void	unite(int *parent, int *size, int i, int j)
{
	int	root_i = find_set(parent, i);
	int	root_j = find_set(parent, j);

	if (root_i != root_j)
	{
		if (size[root_i] < size[root_j])
		{
			parent[root_i] = root_j;
			size[root_j] += size[root_i];
		}
		else
		{
			parent[root_j] = root_i;
			size[root_i] += size[root_j];
		}
	}
}

static int	calculate_points(StringView *sv)
{
	int	res = 0;

	for (size_t i = 0; i < sv->len; ++i)
	{
		if (sv->data[i] == '\n')
			res++;
	}
	if (sv->len > 0 && sv->data[sv->len - 1] != '\n')
		res++;
	return (res);
}

static Point* store_points(Arena *a, StringView *sv, int points_num)
{
	Point	*points = arena_alloc(a, sizeof(Point) * points_num);

	for (int n = 0; sv->len > 0; ++n)
	{
		StringView line = sv_chop(sv, '\n');
		if (line.len == 0)
			continue;
		points[n].x = sv_to_int(sv_chop(&line, ','));
		points[n].y = sv_to_int(sv_chop(&line, ','));
		points[n].z = sv_to_int(line);
	}
	return (points);
}

int	main(void)
{
	Arena	a;

	a = arena_init(PROT_READ | PROT_WRITE);
	char *buffer = arena_alloc(&a, MEMARENA_DEFAULT_SIZE);
	size_t bytes_read = read(0, buffer, MEMARENA_DEFAULT_SIZE);
	StringView sv = { buffer, bytes_read };

	int points_num = calculate_points(&sv);
	Point *points = store_points(&a, &sv, points_num);

	int max_edges = points_num * (points_num - 1) / 2;
	Edge *edges = arena_alloc(&a, sizeof(Edge) * max_edges);
	int edge_count = 0;
	for (int i = 0; i < points_num; ++i)
	{
		for (int j = i + 1; j < points_num; ++j)
		{
			long long dx = points[i].x - points[j].x;
			long long dy = points[i].y - points[j].y;
			long long dz = points[i].z - points[j].z;

			edges[edge_count].u = i;
			edges[edge_count].v = j;
			edges[edge_count].dist_sq = dx * dx + dy * dy + dz * dz;
			edge_count++;
		}
	}

	quicksort_edges(edges, 0, edge_count - 1);

	int *parent = arena_alloc(&a, sizeof(int) * points_num);
	int *size = arena_alloc(&a, sizeof(int) * points_num);
	for (int i = 0; i < points_num; ++i)
	{
		parent[i] = i;
		size[i] = 1;
	}

	int num_components = points_num;
	for (long long i = 0; i < edge_count; ++i)
	{
		int u = edges[i].u;
		int v = edges[i].v;

		if (find_set(parent, u) != find_set(parent, v))
		{
			unite(parent, size, u, v);
			num_components--;
			if (num_components == 1)
			{
				long long result = points[u].x * points[v].x;
				printf("%lld\n", result);
				break;
			}
		}
	}
	arena_free(&a);
	return (0);
}
