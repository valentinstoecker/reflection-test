cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)


project(reflect_test)

# find_package(reflectcpp REQUIRED)
find_package(RapidJSON REQUIRED)
add_executable(reflect_test main.cpp)
target_link_libraries(reflect_test PRIVATE RapidJSON)
install(TARGETS reflect_test)