#include "legacy_humble_allocator.h"
#include "list.h"

#include <list>
#include <vector>
#include <map>
#include <functional>

#define BOOST_TEST_MODULE test_main

#include <boost/test/unit_test.hpp>

#ifdef MEMORY_BLOCK_TRACING
std::atomic_int nonstd::legacy::memory_block::count{};
#endif

template<typename T, size_t N>
using alloc = nonstd::legacy::humble_allocator<T, N>;

struct hard
{
  hard(const char* str, size_t num)
    : str(str), num(num)
  {}
  hard(const hard&) = delete;

  const char * str = nullptr;
  size_t num = 0;
};

BOOST_AUTO_TEST_SUITE(test_suite_main)

  BOOST_AUTO_TEST_CASE(test_in_std_list_of_ints_copying)
  {
    std::list<int, alloc<int, 10>> l1{0,1,2,3,4,5,6,7,8,9};
    std::list<int, alloc<int, 10>> l2(l1);
    std::list<int, alloc<int, 10>> l3(l1.begin(), l1.end());
    BOOST_CHECK(std::equal(l1.begin(), l1.end(), l2.begin(), l2.end()));
    BOOST_CHECK(std::equal(l1.begin(), l1.end(), l3.begin(), l3.end()));
  }

  BOOST_AUTO_TEST_CASE(test_in_std_list_of_ints_moving)
  {
    std::list<int, alloc<int, 10>> l{0,1,2,3,4,5,6,7,8,9};
    std::list<int, alloc<int, 10>> l1{0,1,2,3,4,5,6,7,8,9};
    std::list<int, alloc<int, 10>> l2(std::move(l1));
    BOOST_CHECK(std::equal(l.begin(), l.end(), l2.begin(), l2.end()));
  }

  BOOST_AUTO_TEST_CASE(test_in_std_vector_of_ints_copying)
  {
    std::vector<int, alloc<int, 10>> v1{0,1,2,3,4,5,6,7,8,9};
    std::vector<int, alloc<int, 10>> v2(v1);
    std::vector<int, alloc<int, 10>> v3(v1.begin(), v1.end());
    BOOST_CHECK(std::equal(v1.begin(), v1.end(), v2.begin(), v2.end()));
    BOOST_CHECK(std::equal(v1.begin(), v1.end(), v3.begin(), v3.end()));
  }

  BOOST_AUTO_TEST_CASE(test_in_std_vector_of_ints_moving)
  {
    std::vector<int, alloc<int, 10>> v{0,1,2,3,4,5,6,7,8,9};
    std::vector<int, alloc<int, 10>> v1{0,1,2,3,4,5,6,7,8,9};
    std::vector<int, alloc<int, 10>> v2(std::move(v1));
    BOOST_CHECK(std::equal(v.begin(), v.end(), v2.begin(), v2.end()));
  }

  BOOST_AUTO_TEST_CASE(test_in_std_map_of_int_int_copying)
  {
    std::map<int, int, std::less<int>, alloc<std::pair<const int,int>, 10>> m1
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
    std::map<int, int, std::less<int>, alloc<std::pair<const int, int>, 10>> m2(m1);
    std::map<int, int, std::less<int>, alloc<std::pair<const int, int>, 10>> m3(m1.begin(), m1.end());
    BOOST_CHECK(std::equal(m1.begin(), m1.end(), m2.begin(), m2.end()));
    BOOST_CHECK(std::equal(m1.begin(), m1.end(), m3.begin(), m3.end()));
  }

  BOOST_AUTO_TEST_CASE(test_in_std_map_of_int_int_moving)
  {
    std::map<int, int, std::less<int>, alloc<std::pair<const int, int>, 10>> m
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
    std::map<int, int, std::less<int>, alloc<std::pair<const int, int>, 10>> m1
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
    std::map<int, int, std::less<int>, alloc<std::pair<const int, int>, 10>> m2(std::move(m1));
    BOOST_CHECK(std::equal(m.begin(), m.end(), m2.begin(), m2.end()));
  }

  BOOST_AUTO_TEST_CASE(test_emplace_back_in_std_list_with_non_copyable_humble)
  {
    std::list<hard, alloc<hard, 10>> l2;
    l2.emplace_back("one", 1);
    l2.emplace_back("two", 2);
    l2.emplace_back("three", 3);

    auto it = l2.cbegin();
    BOOST_CHECK(it->num == 1);

    ++it;
    BOOST_CHECK(it->num == 2);

    ++it;
    BOOST_CHECK(it->num == 3);
  }

  BOOST_AUTO_TEST_CASE(test_in_nonstd_list_of_ints_copying)
  {
    nonstd::list<int, alloc<int, 10>> l1{0,1,2,3,4,5,6,7,8,9};
    nonstd::list<int, alloc<int, 10>> l2(l1);
    nonstd::list<int, alloc<int, 10>> l3(l1.begin(), l1.end());
    BOOST_CHECK(std::equal(l1.begin(), l1.end(), l2.begin(), l2.end()));
    BOOST_CHECK(std::equal(l1.begin(), l1.end(), l3.begin(), l3.end()));
  }

  BOOST_AUTO_TEST_CASE(test_in_nonstd_list_of_ints_moving)
  {
    nonstd::list<int, alloc<int, 10>> l{0,1,2,3,4,5,6,7,8,9};
    nonstd::list<int, alloc<int, 10>> l1{0,1,2,3,4,5,6,7,8,9};
    nonstd::list<int, alloc<int, 10>> l2(std::move(l1));
    BOOST_CHECK(std::equal(l.begin(), l.end(), l2.begin(), l2.end()));
  }

  BOOST_AUTO_TEST_CASE(test_emplace_back_in_nonstd_list_with_non_copyable_humble)
  {
    nonstd::list<hard, alloc<hard, 10>> l2;
    l2.emplace_back("one", 1);
    l2.emplace_back("two", 2);
    l2.emplace_back("three", 3);

    auto it = l2.cbegin();
    BOOST_CHECK(it->num == 1);

    ++it;
    BOOST_CHECK(it->num == 2);

    ++it;
    BOOST_CHECK(it->num == 3);
  }

  // initializer_list<hard> required copy-ctor
  // BOOST_AUTO_TEST_CASE(test_initializer_list_in_list_with_non_copyable_humble)
  // {
  //   std::list<hard, alloc<hard, 10>> l1{{"one", 1}, {"two", 2}, {"three", 3}};
  //   std::list<hard, alloc<hard, 10>> l2;
  //   l2.emplace_back("one", 1);
  //   l2.emplace_back("two", 2);
  //   l2.emplace_back("three", 3);
  //   BOOST_CHECK(
  //     std::equal(
  //       l1.begin()
  //       , l1.end()
  //       , l2.begin()
  //       , l2.end()
  //       , [](const hard& lhs, const hard& rhs)
  //         {
  //           return ((lhs.num == rhs.num) && (lhs.str == rhs.str));
  //         }
  //       )
  //     );
  // }

  // following doesn't compile since the Allocator type induces the Container type
  // BOOST_AUTO_TEST_CASE(test_in_list_of_ints_copying_with_different_humble)
  // {
  //   std::list<int, alloc<int, 10>> l1{0,1,2,3,4,5,6,7,8,9};
  //   std::list<int, alloc<int, 11>> l2(l1, alloc<int, 11>());
  //   std::list<int, alloc<int, 11>> l3(l1.begin(), l1.end(), alloc<int, 11>());
  //   BOOST_CHECK(std::equal(l1.begin(), l1.end(), l2.begin(), l2.end()));
  //   BOOST_CHECK(std::equal(l1.begin(), l1.end(), l3.begin(), l3.end()));
  // }

  // following doesn't compile since the Allocator type induces the Container type
  // BOOST_AUTO_TEST_CASE(test_in_list_of_ints_moving_with_different_humble)
  // {
  //   std::list<int, alloc<int, 10>> l{0,1,2,3,4,5,6,7,8,9};
  //   std::list<int, alloc<int, 11>> l1{0,1,2,3,4,5,6,7,8,9};
  //   std::list<int, alloc<int, 11>> l2(std::move(l1), alloc<int, 11>());
  //   BOOST_CHECK(std::equal(l.begin(), l.end(), l2.begin(), l2.end()));
  // }

  // following doesn't compile as expected
  // BOOST_AUTO_TEST_CASE(test_in_list_of_ints_copying_from_humble_to_std)
  // {
  //   std::list<int, alloc<int, 10>> l1{0,1,2,3,4,5,6,7,8,9};
  //   std::list<int> l2(l1);
  //   std::list<int> l3(l1.begin(), l1.end());
  //   BOOST_CHECK(std::equal(l1.begin(), l1.end(), l2.begin(), l2.end()));
  //   BOOST_CHECK(std::equal(l1.begin(), l1.end(), l3.begin(), l3.end()));
  // }

  // following doesn't compile as expected
  // BOOST_AUTO_TEST_CASE(test_in_list_of_ints_moving_from_humble_to_std)
  // {
  //   std::list<int, alloc<int, 10>> l{0,1,2,3,4,5,6,7,8,9};
  //   std::list<int, alloc<int, 10>> l1{0,1,2,3,4,5,6,7,8,9};
  //   std::list<int> l2(std::move(l1));
  //   BOOST_CHECK(std::equal(l.begin(), l.end(), l2.begin(), l2.end()));
  // }

  // following doesn't compile as expected
  // BOOST_AUTO_TEST_CASE(test_in_vector_of_ints_copying_from_humble_to_std)
  // {
  //   std::vector<int, alloc<int, 10>> v1{0,1,2,3,4,5,6,7,8,9};
  //   std::vector<int> v2(v1);
  //   std::vector<int> v3(v1.begin(), v1.end());
  //   BOOST_CHECK(std::equal(v1.begin(), v1.end(), v2.begin(), v2.end()));
  //   BOOST_CHECK(std::equal(v1.begin(), v1.end(), v3.begin(), v3.end()));
  // }

  // following doesn't compile as expected
  // BOOST_AUTO_TEST_CASE(test_in_vector_of_ints_moving_from_humble_to_std)
  // {
  //   std::vector<int, alloc<int, 10>> v{0,1,2,3,4,5,6,7,8,9};
  //   std::vector<int, alloc<int, 10>> v1{0,1,2,3,4,5,6,7,8,9};
  //   std::vector<int> v2(std::move(v1));
  //   BOOST_CHECK(std::equal(v.begin(), v.end(), v2.begin(), v2.end()));
  // }

BOOST_AUTO_TEST_SUITE_END()
