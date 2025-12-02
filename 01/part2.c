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

static inline bool is_reducible(size_t n)
{
	size_t len = count_digits(n);
	for (size_t sub_len = 1; sub_len <= len / 2; ++sub_len)
	{
		if (len % sub_len != 0)
			continue;
		size_t divisor = power_of_ten(len - sub_len);
		size_t sub_chunk = n / divisor;
		size_t reconstructed = 0;
		size_t chunk_multiplier = power_of_ten(sub_len);
		size_t repeats = len / sub_len;
		for (int i = 0; i < repeats; ++i)
			reconstructed = (reconstructed * chunk_multiplier) + sub_chunk;
		if (reconstructed == n)
			return true;
	}
	return false;

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
		int max_digits = count_digits(end);

		for (size_t chunk_len = 1; chunk_len <= max_digits / 2; ++chunk_len)
		{
			size_t seed_start = power_of_ten(chunk_len - 1);
			size_t seed_end = power_of_ten(chunk_len) - 1;
			size_t multiplier = power_of_ten(chunk_len);
			size_t limit_val = power_of_ten(max_digits);

			for (size_t seed = seed_start; seed <= seed_end; ++seed)
			{
				if (is_reducible(seed))
					continue;
				size_t current_val;
				if (__builtin_mul_overflow(seed, multiplier, &current_val))
					continue;
				current_val += seed;

				while (true)
				{
					if (current_val >= limit_val)
						break;
					if (current_val > end)
						break;
					if (current_val >= start)
						sum += current_val;
					size_t next_val;
					if (__builtin_mul_overflow(current_val, multiplier, &next_val))
						break;
					next_val += seed;
					current_val = next_val;
				}
			}
		}
		token = strtok(NULL, DELIMITERS);
	}
	printf("Sum of invalid IDs: %zu\n", sum);
	free(input_file);
	return (0);
}
