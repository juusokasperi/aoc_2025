#include <iostream>
#include <vector>
#include <string>
#include <set>

int main(void)
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);

	std::vector<std::string> grid;
	std::string line;

	while (std::getline(std::cin, line))
	{
		if (!line.empty())
			line.pop_back();
		if (!line.empty())
			grid.push_back(line);
	}

	if (grid.empty())
		return 0;
	std::set<int> currentBeams;
	for (int i = 0; i < grid[0].size(); ++i)
	{
		if (grid[0][i] == 'S')
		{
			currentBeams.insert(i);
			break;
		}
	}

	long long totalSplits = 0;
	int height = grid.size();
	int width = grid[0].size();

	for (int r = 0; r < height; ++r)
	{
		std::set<int> nextBeams;
		for (int col : currentBeams)
		{
			if (col < 0 || col >= width)
				continue;
			char c = grid[r][col];
			if (c == '^')
			{
				totalSplits++;
				nextBeams.insert(col - 1);
				nextBeams.insert(col + 1);
			}
			else
				nextBeams.insert(col);
		}
		currentBeams = nextBeams;
	}

	std::cout << "Total splits: " << totalSplits << "\n";
	return 0;
}
