cmake_minimum_required(VERSION 2.8)

project(cg-test)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

find_package(GMP REQUIRED)
include_directories(${GMP_INCLUDE_DIR})

find_package(Boost COMPONENTS random REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARYDIR})

set(SOURCES
	orientation.cpp
	has_intersection.cpp
	contains.cpp
	convex_hull.cpp
	dynamic_convex_hull.cpp
	convex.cpp
	visibility3.cpp
	shortest_path.cpp
   minkowski_sum.cpp
)

add_executable(cg-test ${SOURCES})
target_link_libraries(cg-test ${GTEST_BOTH_LIBRARIES} ${GMP_LIBRARIES})

file(GLOB_RECURSE HEADERS "*.h")
add_custom_target(cg_test_headers SOURCES ${HEADERS})
