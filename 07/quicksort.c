#include "structs.h"
#include <unistd.h>

#define SWAP_EDGE(a,b) { Edge t = *(a); *(a) = *(b); *(b) = t; }

static void	insertion_sort_edges(Edge *arr, int low, int high)
{
	for (int i = low + 1; i <= high; ++i)
	{
		Edge key = arr[i];
		int j = i - 1;
		while (j >= low && arr[j].dist_sq > key.dist_sq)
		{
			arr[j + 1] = arr[j];
			j--;
		}
		arr[j + 1] = key;
	}
}

static void	median_of_three(Edge *arr, int low, int high)
{
	int mid = low + (high - low) / 2;
	if (arr[mid].dist_sq < arr[low].dist_sq)
		SWAP_EDGE(&arr[low], &arr[mid]);
	if (arr[high].dist_sq < arr[low].dist_sq)
		SWAP_EDGE(&arr[low], &arr[high]);
	if (arr[high].dist_sq < arr[mid].dist_sq)
		SWAP_EDGE(&arr[mid], &arr[high]);
	SWAP_EDGE(&arr[mid], &arr[low]);
}

static int	partition(Edge *arr, int low, int high)
{
	median_of_three(arr, low, high);
	long long pivot = arr[low].dist_sq;
	int i = low - 1;
	int j = high + 1;
	while (1)
	{
		do { i++; } while (arr[i].dist_sq < pivot);
		do { j--; } while (arr[j].dist_sq > pivot);
		if (i >= j) return (j);
		SWAP_EDGE(&arr[i], &arr[j]);
	}
}

void	quickselect_edges(Edge *arr, int low, int high, int k)
{
	while (low < high)
	{
		int p = partition(arr, low, high);
		if (k <= p)
			high = p;
		else
			low = p + 1;
	}
}

void	quicksort_edges(Edge *arr, int low, int high)
{
	while (low < high)
	{
		if (high - low < 16)
			return (insertion_sort_edges(arr, low, high));
		int p = partition(arr, low, high);
		if (p - low < high - p)
		{
			quicksort_edges(arr, low, p);
			low = p + 1;
		}
		else
		{
			quicksort_edges(arr, p + 1, high);
			high = p;
		}
	}
}
