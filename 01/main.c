#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define DELIMITERS ",\n\r"

static char *read_file(const char *input_file)
{
	FILE* f = fopen(input_file, "r");
	if (!f)
	{
		perror("Failed to open file");
		return (NULL);
	}
	
	fseek(f, 0, SEEK_END);
	size_t len = ftell(f);
	rewind(f);

	char *buffer = malloc(len + 1);
	if (!buffer)
	{
		perror("Malloc failed");
		fclose(f);
		return (NULL);
	}

	fread(buffer, 1, len, f);
	buffer[len] = '\0';
	fclose(f);
	return (buffer);
}

static inline size_t count_digits(size_t n)
{
	if (n == 0)
		return (1);

	size_t count = 0;
	while (n > 0)
	{
		n /= 10;
		count++;
	}
	return (count);
}

static inline size_t power_of_ten(size_t n)
{
	size_t result = 1;
	for (size_t i = 0; i < n; ++i)
		result *= 10;
	return (result);
}

static inline size_t get_start_generator(size_t range_start)
{
	int digits = count_digits(range_start);

	if (digits % 2 != 0)
	{
		int needed_digits = (digits + 1) / 2;
		return power_of_ten(needed_digits - 1);
	}
	else
		return range_start / power_of_ten(digits / 2);
}

static inline size_t make_silly(size_t n)
{
	size_t temp = n;
	size_t multiplier = 1;
	while (temp > 0)
	{
		multiplier *= 10;
		temp /= 10;
	}
	return ((n * multiplier) + n);
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Error: Run with %s <input_file>\n", argv[0]);
		return (1);
	}

	char *input_file = read_file(argv[1]);
	if (!input_file)
		return (1);
	size_t sum = 0;
	char *token = strtok(input_file, DELIMITERS);

	while (token != NULL)
	{
		char *dash_ptr;
		size_t start = strtoul(token, &dash_ptr, 10);
		if (*dash_ptr != '-')
		{
			token = strtok(NULL, DELIMITERS);
			continue;
		}

		size_t end = strtoul(dash_ptr + 1, NULL, 10);
		size_t g_start = get_start_generator(start);
		int max_digits = count_digits(end);
		size_t prev_silly = 0;
		
		for (size_t g = g_start; ; ++g)
		{
			size_t silly = make_silly(g);

			if (silly < g || silly < prev_silly)
				break;
			prev_silly = silly;
			if (count_digits(silly) > max_digits)
				break;
			if (silly >= start && silly <= end)
				sum += silly;
		}
		token = strtok(NULL, DELIMITERS);
	}
	printf("Sum of invalid IDs: %zu\n", sum);
	free(input_file);
	return (0);
}
