cmake_minimum_required(VERSION 3.23.1)

project(WinConsoles)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DWINCONSOLES_ERROR_BOXES -DUNICODE)

add_subdirectory("src" "lib")
add_subdirectory("src/ConsoleProcess" "lib/ConsoleProcess")
add_subdirectory("tests")

file(WRITE src/ConsoleLoc.h "#pragma once\nconstexpr char CONSOLE_PATH[]{\"${CMAKE_BINARY_DIR}/lib/ConsoleProcess\"};")

# file(COPY src/WinConsoles.h DESTINATION ${CMAKE_BINARY_DIR}/include/)
