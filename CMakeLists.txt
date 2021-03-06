cmake_minimum_required(VERSION 3.2)

project(allocator VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

configure_file(version.h.in autoversion.h)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  )

add_executable(allocator main.cpp)
add_executable(test_legacy_humble_allocator test_legacy_humble_allocator.cpp)
add_executable(test_pmr_humble_allocator test_pmr_humble_allocator.cpp)

set_target_properties(
  allocator
  test_legacy_humble_allocator
  PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
  )

set_target_properties(
  test_pmr_humble_allocator
  PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
  )


set_target_properties(test_legacy_humble_allocator PROPERTIES
  COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
  INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
  INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}
  )

target_link_libraries(
  test_legacy_humble_allocator
  ${Boost_LIBRARIES}
  )

set_target_properties(test_pmr_humble_allocator PROPERTIES
  COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
  INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
  INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR}
  )

target_link_libraries(
  test_pmr_humble_allocator
  ${Boost_LIBRARIES}
  )

install(TARGETS allocator RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_PACKAGE_CONTACT "ad.beregovoy@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A custom allocator examples")
set(CPACK_PACKAGE_DESCRIPTION "Quick and dirty example of custom allocators")

set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
set(CPACK_DEBIAN_PACKAGE_SECTION "Development")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Anton Beregovoy <${CPACK_PACKAGE_CONTACT}>")

include(CPack)

enable_testing()
add_test(allocator_tests test_legacy_humble_allocator)
add_test(allocator_tests test_pmr_humble_allocator)
