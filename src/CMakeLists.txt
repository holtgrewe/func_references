cmake_minimum_required(VERSION 2.8)
enable_testing()

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

find_package(GTest REQUIRED)
find_package(LibTooling REQUIRED)
add_definitions(${Tooling_DEFINES})

include_directories(${GTEST_INCLUDE_DIRS} ${Tooling_INCLUDE_DIRS})
add_executable(my_test test.cpp)
target_link_libraries(my_test ${GTEST_BOTH_LIBRARIES} ${Tooling_LIBRARY})
add_test(the_tests my_test)