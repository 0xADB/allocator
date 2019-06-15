#include "legacy_humble_allocator.h"

#include <iostream>
#include <map>
#include <algorithm>
#include <type_traits>

#include "list.h"

#ifdef MEMORY_BLOCK_TRACING
std::atomic_int nonstd::memory::memory_block::count{};
#endif

template<typename C, typename Enable = void>
struct printer
  {
    template<typename G>
      void operator()(G generator, std::ostream& stream = std::cout)
      {
	auto m = C();
	std::generate_n(std::back_inserter(m), 10, generator);

	stream << '\n';
	for (const auto& p : m)
	  stream << p << '\n';
      }
  };

template<typename C>
  struct printer<
    C
    , std::enable_if_t<
	std::is_same<typename C::value_type, std::pair<const typename C::key_type, typename C::mapped_type>>::value
      >
    >
  {
    template<typename G>
      void operator()(G generator, std::ostream& stream = std::cout)
      {
	auto m = C();
	std::generate_n(std::inserter(m, m.end()), 10, generator);

	stream << '\n';
	for (const auto& p : m)
	  stream << p.first << " " << p.second << '\n';
      }
  };

template<typename C>
  struct printer<
    C
    , std::enable_if_t<
	std::is_same<typename C::key_type, typename C::value_type>::value
      >
    >
  {
    template<typename G>
      void operator()(G generator, std::ostream& stream = std::cout)
      {
	auto m = C();
	std::generate_n(std::inserter(m, m.end()), 10, generator);

	stream << '\n';
	for (const auto& p : m)
	  stream << p << '\n';
      }
  };

int main(int, char **)
{
  std::ios_base::sync_with_stdio(false);

  auto factorial_pair_generator = [n = 1, i = 0]() mutable
	{
	  if (i > 0) n *= i;
	  return std::make_pair(i++, n);
	};

  auto factorial_value_generator = [n = 1, i = 0]() mutable
	{
	  if (i > 0) n *= i;
	  i++;
	  return n;
	};

  {
    // standard container with standard allocator
    printer<
      std::map<
	int
	, int
	>
      >{}(factorial_pair_generator);
  }

  {
    // standard container with humble allocator
    printer<
      std::map<
	int
	, int
	, std::less<int>
	, nonstd::legacy_allocator::humble<std::pair<const int, int>, 10>
	>
      >{}(factorial_pair_generator);
  }

  {
    //custom container with std::allocator
    printer<
      nonstd::list<
	int
	>
      >{}(factorial_value_generator);
  }

  {
    // custom container with humble allocator
    printer<
      nonstd::list<
	int
	, nonstd::legacy_allocator::humble<int, 10>
	>
      >{}(factorial_value_generator);
  }

  return 0;
}
