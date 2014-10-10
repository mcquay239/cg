cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(visibility_example)

add_executable(visibility_fast visibility_fast.cpp)
target_link_libraries(visibility_fast visualization ${GMP_LIBRARIES})
