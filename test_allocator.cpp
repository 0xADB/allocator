#include "humble_allocator.h"

#include <iostream>
#include <sstream>
#include <list>

using namespace std::string_literals;

#define BOOST_TEST_MODULE test_main

#include <boost/test/unit_test.hpp>

// FIXME: feeling like I test list(vector, map) here instead of allocator...

BOOST_AUTO_TEST_SUITE(test_suite_main)

  BOOST_AUTO_TEST_CASE(test_list_copy)
  {
    std::list<int, nonstd::allocator::humble<int, 10>> l1{0,1,2,3,4,5,6,7,8,9};
    std::list<int, nonstd::allocator::humble<int, 10>> l2(l1);
    std::list<int, nonstd::allocator::humble<int, 10>> l3(l1.begin(), l1.end());
    BOOST_CHECK(std::equal(l1.begin(), l1.end(), l2.begin(), l2.end()));
    BOOST_CHECK(std::equal(l1.begin(), l1.end(), l3.begin(), l3.end()));

  }

  BOOST_AUTO_TEST_CASE(test_list_erase)
  {
    std::list<int, nonstd::allocator::humble<int, 10>> l1{0,1,2,3,4,5,6,7,8,9};
    auto it = l1.begin();
    it = l1.erase(it);
    ++it;
    BOOST_CHECK(*it == 3);
    std::list<int, nonstd::allocator::humble<int, 10>> l2{1,3,4,5,6,7,8,9};
    BOOST_CHECK(std::equal(l1.begin(), l1.end(), l2.begin(), l2.end()));
  }

BOOST_AUTO_TEST_SUITE_END()
