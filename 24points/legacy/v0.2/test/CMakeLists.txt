cmake_minimum_required(VERSION 3.14)
project(my_project)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

find_package(GTest REQUIRED)

add_executable(
    24points_test
    24points_test.cpp
)
target_link_libraries(
    24points_test
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(24points_test)
