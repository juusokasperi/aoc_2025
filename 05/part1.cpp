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

	std::string line;
	std::vector<__int128_t> sums;
	std::vector<__int128_t> prods;
	bool firstRow = true;

	while (std::getline(std::cin, line) && !line.empty())
	{
		if (line.find_first_of("+*") != std::string::npos)
			break;

		std::stringstream ss(line);
		long long val;
		int col = 0;
		
		while (ss >> val)
		{
			if (firstRow)
			{
				sums.push_back(val);
				prods.push_back(val);
			}
			else
			{
				if (col < sums.size())
				{
					sums[col] += val;
					prods[col] *= val;
				}
			}
			col++;
		}
		firstRow = false;
	}

	std::stringstream opStream(line);
	char op;
	int col = 0;
	__int128_t totalRes = 0;

	while (opStream >> op)
	{
		if (col >= sums.size())
			break;
		__int128_t colResult = 0;
		if (op == '+')
			colResult = sums[col];
		else if (op == '*')
			colResult = prods[col];
		totalRes += colResult;
		col++;
	}
	std::cout << "Result: " << totalRes << "\n";
}
