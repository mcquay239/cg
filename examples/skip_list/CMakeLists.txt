find_package(Boost COMPONENTS chrono system REQUIRED)

add_executable(skip_list skip_list.cpp)
target_link_libraries(skip_list visualization ${Boost_LIBRARIES})
