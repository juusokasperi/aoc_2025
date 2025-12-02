#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

static inline bool is_silly_pattern(size_t n)
{
	char buffer[128];
	sprintf(buffer, "%zu", n);
	size_t len = strlen(buffer);
	if (len % 2 != 0)
		return false;
	
	size_t half_len = len / 2;
	if (strncmp(buffer, buffer + half_len, half_len) == 0)
		return true;
	return false;
}

static inline char *read_file(const char *input_file)
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
	char *token = strtok(input_file, ",\n\r");

	while (token != NULL)
	{
		size_t start;
		size_t end;

		if (sscanf(token, "%zu-%zu", &start, &end) == 2)
		{
			for (size_t i = start; i <= end; ++i)
			{
				if (is_silly_pattern(i))
					sum += i;
			}
		}
		token = strtok(NULL, ",\n\r");
	}
	printf("Sum of invalid IDs: %zu\n", sum);
	free(input_file);
	return (0);
}
