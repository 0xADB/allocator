
#include "humble_allocator.h"

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

int main(int, char **)
{
  std::ios_base::sync_with_stdio(false);

  auto fact_o_rator = [n = 1, i = 0]() mutable
	{
	  if (i > 0) n *= i;
	  return std::make_pair(i++, n);
	};

  auto m = std::map<int, int>();
  std::generate_n(std::inserter(m, m.end()), 10, fact_o_rator);

  std::cout << '\n';
  for (const auto& p : m)
    std::cout << p.first << " - " << p.second << '\n';
  std::cout << std::endl;

  auto humbleM = std::map<int, int, std::less<int>, all_o_cator::humble<std::pair<const int, int>, 10>>();
  std::generate_n(std::inserter(humbleM, humbleM.end()), 10, fact_o_rator);

  std::cout << '\n';
  for (const auto& p : humbleM)
    std::cout << p.first << " - " << p.second << '\n';
  std::cout << std::endl;

  return 0;
}
