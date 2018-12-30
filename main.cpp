#include "humble_allocator.h"

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <type_traits>

#include "list.h"

template <typename> struct is_pair : std::false_type{};
template <typename T, typename U> struct is_pair<std::pair<T, U>> : std::true_type{};

template<typename ContainerT, typename ValueT = typename ContainerT::value_type>
struct printer
{
  template<typename GeneratorT>
    void operator()(GeneratorT generator, std::ostream& stream = std::cout)
  {
    auto m = ContainerT();
    std::generate_n(std::inserter(m, m.end()), 10, generator);

    stream << '\n';
    for (const auto& p : m)
      stream << p.first << " " << p.second << '\n';
  }
};

template<typename ContainerT>
  struct printer<
    ContainerT
    , std::pair<typename ContainerT::key_type, typename ContainerT::mapped_type>
    >
{
  template<typename GeneratorT>
    void operator()(GeneratorT generator, std::ostream& stream = std::cout)
    {
      auto m = ContainerT();
      std::generate_n(std::inserter(m, m.end()), 10, generator);

      stream << '\n';
      for (const auto& p : m)
	stream << p.first << " " << p.second << '\n';
    }
};

int main(int, char **)
{
  std::ios_base::sync_with_stdio(false);

  auto factorial_generator = [n = 1, i = 0]() mutable
	{
	  if (i > 0) n *= i;
	  return std::make_pair(i++, n);
	};

  // standard container with standard allocator
  printer<
    std::map<
      int
      , int
      >
    >()(factorial_generator);

  // standard container with humble allocator
  printer<
    std::map<
      int
      , int
      , std::less<int>
      , nonstd::allocator::humble<std::pair<const int, int>, 10>
      >
    >()(factorial_generator);

  nonstd::list<int> nl;


  return 0;
}
