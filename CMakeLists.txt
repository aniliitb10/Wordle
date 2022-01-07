cmake_minimum_required(VERSION 3.21)
project(Wordle)

set(CMAKE_CXX_STANDARD 20)
include_directories(include)

# including tests
file(GLOB gTestSource "test/*.cpp")

add_executable(Wordle main.cpp ${gTestSource})
target_link_libraries(Wordle gtest pthread)