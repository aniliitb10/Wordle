cmake_minimum_required(VERSION 3.21)
project(Wordle)

set(CMAKE_CXX_STANDARD 20)
include_directories(include)

# including tests
file(GLOB gTestSource "test/*.cpp")

# create executables
add_executable(Wordle main.cpp ${gTestSource})
add_executable(WordleClient wordle_client_main.cpp)

target_link_libraries(Wordle gtest pthread)