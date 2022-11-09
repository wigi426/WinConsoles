
cmake_minimum_required(VERSION 3.10)

project(testProg)

add_definitions(-DUNICODE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(theLib STATIC "theLib.cpp" "theLib.hpp")

add_executable(testProg "testProg.cpp")
target_link_libraries(testProg theLib)


