
cmake_minimum_required(VERSION 3.10)

project(inheritPractice)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_definitions(-DUNICODE)
add_executable(parent parent.cpp)
add_executable(child child.cpp)
