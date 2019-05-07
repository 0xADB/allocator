#include "humble_allocator.h"
#include "list.h"

#include <iostream>
#include <sstream>
#include <list>
#include <vector>
#include <map>
#include <functional>

using namespace std::string_literals;

#define BOOST_TEST_MODULE test_main

#include <boost/test/unit_test.hpp>

#ifdef MEMORY_BLOCK_TRACING
std::atomic_int nonstd::memory::memory_block::count{};
#endif

BOOST_AUTO_TEST_SUITE(test_suite_main)

  BOOST_AUTO_TEST_CASE(test_allocator_in_list_of_ints_copying)
  {
    std::list<int, nonstd::legacy_allocator::humble<int, 10>> l1{0,1,2,3,4,5,6,7,8,9};
    std::list<int, nonstd::legacy_allocator::humble<int, 10>> l2(l1);
    std::list<int, nonstd::legacy_allocator::humble<int, 10>> l3(l1.begin(), l1.end());
    BOOST_CHECK(std::equal(l1.begin(), l1.end(), l2.begin(), l2.end()));
    BOOST_CHECK(std::equal(l1.begin(), l1.end(), l3.begin(), l3.end()));
  }

  BOOST_AUTO_TEST_CASE(test_allocator_in_list_of_ints_moving)
  {
    std::list<int, nonstd::legacy_allocator::humble<int, 10>> l{0,1,2,3,4,5,6,7,8,9};
    std::list<int, nonstd::legacy_allocator::humble<int, 10>> l1{0,1,2,3,4,5,6,7,8,9};
    std::list<int, nonstd::legacy_allocator::humble<int, 10>> l2(std::move(l1));
    BOOST_CHECK(std::equal(l.begin(), l.end(), l2.begin(), l2.end()));
  }

  BOOST_AUTO_TEST_CASE(test_allocator_in_vector_of_ints_copying)
  {
    std::vector<int, nonstd::legacy_allocator::humble<int, 10>> v1{0,1,2,3,4,5,6,7,8,9};
    std::vector<int, nonstd::legacy_allocator::humble<int, 10>> v2(v1);
    std::vector<int, nonstd::legacy_allocator::humble<int, 10>> v3(v1.begin(), v1.end());
    BOOST_CHECK(std::equal(v1.begin(), v1.end(), v2.begin(), v2.end()));
    BOOST_CHECK(std::equal(v1.begin(), v1.end(), v3.begin(), v3.end()));
  }

  BOOST_AUTO_TEST_CASE(test_allocator_in_vector_of_ints_moving)
  {
    std::vector<int, nonstd::legacy_allocator::humble<int, 10>> v{0,1,2,3,4,5,6,7,8,9};
    std::vector<int, nonstd::legacy_allocator::humble<int, 10>> v1{0,1,2,3,4,5,6,7,8,9};
    std::vector<int, nonstd::legacy_allocator::humble<int, 10>> v2(std::move(v1));
    BOOST_CHECK(std::equal(v.begin(), v.end(), v2.begin(), v2.end()));
  }

  BOOST_AUTO_TEST_CASE(test_allocator_in_map_of_int_int_copying)
  {
    std::map<int, int, std::less<int>, nonstd::legacy_allocator::humble<std::pair<const int,int>, 10>> m1
    {
        {0,0}
      , {1,1}
      , {2,2}
      , {3,3}
      , {4,4}
      , {5,5}
      , {6,6}
      , {7,7}
      , {8,8}
      , {9,9}
    };
    std::map<int, int, std::less<int>, nonstd::legacy_allocator::humble<std::pair<const int, int>, 10>> m2(m1);
    std::map<int, int, std::less<int>, nonstd::legacy_allocator::humble<std::pair<const int, int>, 10>> m3(m1.begin(), m1.end());
    BOOST_CHECK(std::equal(m1.begin(), m1.end(), m2.begin(), m2.end()));
    BOOST_CHECK(std::equal(m1.begin(), m1.end(), m3.begin(), m3.end()));
  }

  BOOST_AUTO_TEST_CASE(test_allocator_in_map_of_int_int_moving)
  {
    std::map<int, int, std::less<int>, nonstd::legacy_allocator::humble<std::pair<const int, int>, 10>> m
    {
        {0,0}
      , {1,1}
      , {2,2}
      , {3,3}
      , {4,4}
      , {5,5}
      , {6,6}
      , {7,7}
      , {8,8}
      , {9,9}
    };
    std::map<int, int, std::less<int>, nonstd::legacy_allocator::humble<std::pair<const int, int>, 10>> m1
    {
        {0,0}
      , {1,1}
      , {2,2}
      , {3,3}
      , {4,4}
      , {5,5}
      , {6,6}
      , {7,7}
      , {8,8}
      , {9,9}
    };
    std::map<int, int, std::less<int>, nonstd::legacy_allocator::humble<std::pair<const int, int>, 10>> m2(std::move(m1));
    BOOST_CHECK(std::equal(m.begin(), m.end(), m2.begin(), m2.end()));
  }
BOOST_AUTO_TEST_SUITE_END()
