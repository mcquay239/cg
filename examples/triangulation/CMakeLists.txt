cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(triangulation-example)

add_executable(triangulation triangulation.cpp)
target_link_libraries(triangulation visualization ${GMP_LIBRARIES})
