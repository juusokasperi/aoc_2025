#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

const int BATCH_SIZE = 32 * 1024; // 256kB

struct Range {
	size_t start;
	size_t end;
	bool operator<(const Range& other) const { return start < other.start; }
};

int main(int argc, char **argv)
{
	(void)argv;
	if (argc != 1)
		return 1;

	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);

	std::vector<Range> rawRanges;
	std::string line;

	while (std::getline(std::cin, line) && !line.empty())
	{
		size_t dashPos = line.find('-');
		if (dashPos != std::string::npos)
		{
			size_t s = std::stoll(line.substr(0, dashPos));
			size_t e = std::stoll(line.substr(dashPos + 1));
			rawRanges.push_back({s, e});
		}
	}

	if (rawRanges.empty())
	{
		std::cout << "0\n";
		return 0;
	}

	std::sort(rawRanges.begin(), rawRanges.end());
	std::vector<Range> ranges;
	ranges.push_back(rawRanges[0]);
	for (size_t i = 0; i < rawRanges.size(); ++i)
	{
		Range &current = ranges.back();
		Range &next = rawRanges[i];
		if (next.start <= current.end + 1)
			current.end = std::max(current.end, next.end);
		else
			ranges.push_back(next);
	}
	
	std::vector<size_t> batch;
	batch.reserve(BATCH_SIZE);

	size_t id;
	size_t freshCount = 0;
	auto processBatch = [&](std::vector<size_t>& sortedIDs)
	{
		size_t rIdx = 0;
		size_t idIdx = 0;
		size_t numRanges = ranges.size();
		size_t numIds = sortedIDs.size();

		while (idIdx < numIds && rIdx < numRanges)
		{
			size_t currentId = sortedIDs[idIdx];
			const Range& currentRange = ranges[rIdx];
			if (currentId < currentRange.start)
				idIdx++;
			else if (currentId > currentRange.end)
				rIdx++;
			else
			{
				freshCount++;
				idIdx++;
			}
		}
	};

	while (std::cin >> id)
	{
		batch.push_back(id);
		if (batch.size() == BATCH_SIZE)
		{
			std::sort(batch.begin(), batch.end());
			processBatch(batch);
			batch.clear();
		}
	}

	if (!batch.empty())
	{
		std::sort(batch.begin(), batch.end());
		processBatch(batch);
	}

	std::cout << freshCount << "\n";
	return 0;
}
