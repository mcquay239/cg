cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(visibility-example)

add_executable(visibility3 visibility3.cpp)
target_link_libraries(visibility3 visualization ${GMP_LIBRARIES})

add_executable(shortest_path shortest_path.cpp)
target_link_libraries(shortest_path visualization ${GMP_LIBRARIES})

add_executable(minkowski minkowski.cpp)
target_link_libraries(minkowski visualization ${GMP_LIBRARIES})

