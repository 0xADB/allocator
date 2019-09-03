#include "pmr_memory_block.h"
#include "pmr_list.h"

#include <list>
#include <vector>
#include <map>
#include <functional>

#define BOOST_TEST_MODULE test_main

#include <boost/test/unit_test.hpp>


#if __cplusplus > 201402L

#ifdef MEMORY_BLOCK_TRACING
std::atomic_int nonstd::pmr::memory_block::count{};
#endif

#endif // __cplusplus > 201402L

using alloc = std::pmr::polymorphic_allocator<std::byte>;

template<size_t N>
using memblock = nonstd::pmr::memory_block<N>;

BOOST_AUTO_TEST_SUITE(test_suite_main)

  BOOST_AUTO_TEST_CASE(test_in_list_contruction_and_destruction)
  {
    nonstd::pmr::memory_block<10> m1;
    {
      std::pmr::list<int> l1({0,1,2,3,4,5,6,7,8,9}, &m1);
      std::list<int> l2{0,1,2,3,4,5,6,7,8,9};
      BOOST_CHECK(std::equal(l1.begin(), l1.end(), l2.begin(), l2.end()));
    }
  }

  BOOST_AUTO_TEST_CASE(test_in_list_of_ints_copying)
  {
    nonstd::pmr::memory_block<10> m1;
    nonstd::pmr::memory_block<10> m2;
    nonstd::pmr::memory_block<10> m3;
    std::pmr::list<int> l1({0,1,2,3,4,5,6,7,8,9}, &m1);
    std::pmr::list<int> l2(l1, &m2);
    std::pmr::list<int> l3(l1.begin(), l1.end(), &m3);
    BOOST_CHECK(std::equal(l1.begin(), l1.end(), l2.begin(), l2.end()));
    BOOST_CHECK(std::equal(l1.begin(), l1.end(), l3.begin(), l3.end()));
  }

  BOOST_AUTO_TEST_CASE(test_in_list_of_ints_moving)
  {
    nonstd::pmr::memory_block<10> m1;
    nonstd::pmr::memory_block<10> m2;
    std::pmr::list<int> l({0,1,2,3,4,5,6,7,8,9}, &m1);
    std::pmr::list<int> l1({0,1,2,3,4,5,6,7,8,9}, &m2);
    std::pmr::list<int> l2(std::move(l1));
    BOOST_CHECK(std::equal(l.begin(), l.end(), l2.begin(), l2.end()));
    //BOOST_CHECK(l2.get_allocator() == &m2);
  }

  BOOST_AUTO_TEST_CASE(test_in_vector_of_ints_copying)
  {
    nonstd::pmr::memory_block<10> m1;
    nonstd::pmr::memory_block<10> m2;
    nonstd::pmr::memory_block<10> m3;
    std::pmr::vector<int> v1({0,1,2,3,4,5,6,7,8,9}, &m1);
    std::pmr::vector<int> v2(v1, &m2);
    std::pmr::vector<int> v3(v1.begin(), v1.end(), &m3);
    BOOST_CHECK(std::equal(v1.begin(), v1.end(), v2.begin(), v2.end()));
    BOOST_CHECK(std::equal(v1.begin(), v1.end(), v3.begin(), v3.end()));
  }

  BOOST_AUTO_TEST_CASE(test_in_vector_of_ints_moving)
  {
    nonstd::pmr::memory_block<10> m1;
    nonstd::pmr::memory_block<10> m2;
    std::pmr::vector<int> v({0,1,2,3,4,5,6,7,8,9}, &m1);
    std::pmr::vector<int> v1({0,1,2,3,4,5,6,7,8,9}, &m2);
    std::pmr::vector<int> v2(std::move(v1));
    BOOST_CHECK(std::equal(v.begin(), v.end(), v2.begin(), v2.end()));
    //BOOST_CHECK(v2.get_allocator() == &m2);
  }

  BOOST_AUTO_TEST_CASE(test_in_map_of_int_int_copying)
  {
    nonstd::pmr::memory_block<10> mb1;
    nonstd::pmr::memory_block<10> mb2;
    nonstd::pmr::memory_block<10> mb3;
    std::pmr::map<int, int> m1
    ({
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
    }
    , &mb1
    );
    std::pmr::map<int, int> m2(m1, &mb2);
    std::pmr::map<int, int> m3(m1.begin(), m1.end(), &mb3);
    BOOST_CHECK(std::equal(m1.begin(), m1.end(), m2.begin(), m2.end()));
    BOOST_CHECK(std::equal(m1.begin(), m1.end(), m3.begin(), m3.end()));
  }

  BOOST_AUTO_TEST_CASE(test_in_map_of_int_int_moving)
  {
    nonstd::pmr::memory_block<10> mb1;
    nonstd::pmr::memory_block<10> mb2;
    std::pmr::map<int, int> m
    ({
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
    }
    , &mb1
    );
    std::pmr::map<int, int> m1
    ({
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
    }
    , &mb2
    );
    std::pmr::map<int, int> m2(std::move(m1));
    BOOST_CHECK(std::equal(m.begin(), m.end(), m2.begin(), m2.end()));
  }

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
