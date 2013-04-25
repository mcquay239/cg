cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(operations-example)

add_executable(orientation_contour_ccw orientation_contour_ccw.cpp)
target_link_libraries(orientation_contour_ccw visualization ${GMP_LIBRARIES})
