cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(operations-example)

add_executable(ccw_contour_convex ccw_contour_convex.cpp)
target_link_libraries(ccw_contour_convex visualization ${GMP_LIBRARIES})
