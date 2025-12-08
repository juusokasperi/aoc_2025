#include <iostream>
#include <vector>
#include <string>
#include <sstream>

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

int main()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);

	std::vector<std::string> grid;
	std::string line;
	size_t maxWidth = 0;

	while (std::getline(std::cin, line))
	{
		grid.push_back(line);
		maxWidth = std::max(maxWidth, line.length());
	}
	while (!grid.empty() && grid.back().empty())
		grid.pop_back();
	
	if (grid.empty())
		return 0;

	for (auto &row : grid)
		row.resize(maxWidth, ' ');

	int numRows = grid.size();
	int opRowIdx = numRows - 1;

	__int128_t grandTotal = 0;

	std::vector<__int128_t> blockNums;
	char blockOp = 0;
	bool insideBlock = false;

	auto processBlock = [&]()
	{
		if (blockNums.empty())
			return;
		__int128_t blockRes = 0;
		if (blockOp == 0)
			blockOp = '+';
		if (blockOp == '+')
		{
			blockRes = 0;
			for (auto n : blockNums)
				blockRes += n;
		} 
		else if (blockOp == '*')
		{
			blockRes = 1;
			for (auto n : blockNums)
				blockRes *= n;
		}

		grandTotal += blockRes;
	};

	for (size_t col = 0; col < maxWidth; ++col)
	{
		bool isSpaceCol = true;
		std::string colDigits = "";
		for (int row = 0; row < opRowIdx; ++row)
		{
			char c = grid[row][col];
			if (c != ' ')
			{
				isSpaceCol = false;
				colDigits += c;
			}
		}
		
		char opChar = grid[opRowIdx][col];
		if (opChar != ' ')
			isSpaceCol = false;
		if (isSpaceCol)
		{
			if (insideBlock)
			{
				processBlock();
				blockNums.clear();
				blockOp = 0;
				insideBlock = false;
			}
		} 
		else
		{
			insideBlock = true;
			if (!colDigits.empty())
			{
				__int128_t num = 0;
				for (char c : colDigits)
					num = num * 10 + (c - '0');
				blockNums.push_back(num);
			}
			if (opChar == '+' || opChar == '*')
				blockOp = opChar;
		}
	}

	if (insideBlock)
		processBlock();


	std::cout << "Result: " << grandTotal  << "\n";
}
