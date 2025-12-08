#include <iostream>
#include <vector>
#include <string>
#include <set>

std::ostream &operator<<(std::ostream &os, __int128_t n)
{
	if (n == 0)
		return os << "0";
	unsigned __int128 u;
	if (n < 0)
	{
		os << "-";
		u = -(unsigned __int128)n;
	}
	else
		u = (unsigned __int128)n;
	std::string s;
	while (u > 0)
	{
		s += (char)('0' + (u % 10));
		u /= 10;
	}
	for (int i = s.length() - 1; i >= 0; i--)
		os << s[i];
	return os;
}

int main(void)
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);

	std::vector<std::string> grid;
	std::string line;

	while (std::getline(std::cin, line))
	{
		if (!line.empty())
			grid.push_back(line);
	}

	if (grid.empty())
		return 0;

	int height = grid.size();
	int width = grid[0].size();

	std::vector<__int128_t> currentTimelines(width, 0);
	int startRow = 0;
	bool foundStart = false;

	for (int r = 0; r < height; ++r)
	{
		for (int c = 0; c < width; ++c)
		{
			if (grid[r][c] == 'S')
			{
				currentTimelines[c] = 1;
				startRow = r;
				foundStart = true;
				break;
			}
		}
		if (foundStart)
			break;
	}

	for (int r = startRow; r < height; ++r)
	{
		std::vector<__int128_t> nextTimelines(width, 0);

		for (int c = 0; c < width; ++c)
		{
			if (currentTimelines[c] == 0)
				continue;
			__int128_t count = currentTimelines[c];
			char tile = grid[r][c];
			if (tile == '^')
			{
				if (c > 0)
					nextTimelines[c - 1] += count;
				if (c < width - 1)
					nextTimelines[c + 1] += count;
			}
			else
				nextTimelines[c] += count;
		}
		currentTimelines = nextTimelines;
	}

	__int128_t total = 0;
	for (const auto &val : currentTimelines)
		total += val;
	std::cout << "Result: " << total << "\n";
	return 0;
}
