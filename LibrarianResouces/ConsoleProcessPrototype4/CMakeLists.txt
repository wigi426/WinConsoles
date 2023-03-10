cmake_minimum_required(VERSION 3.10)

project(ConsolePrototype)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(parent new_parent.cpp)
add_executable(console new_console.cpp Win32Helpers.h Win32Helpers.cpp)
add_executable(test main.cpp)

if(MSVC)
    target_compile_options(console BEFORE PRIVATE /W4 /WX)
else()
    target_compile_options(console BEFORE PRIVATE -Wall -Werror -pedantic -Wextra)
endif()

target_include_directories(parent PRIVATE $ENV{MY_CPP_HEADERS})

target_compile_definitions(console PRIVATE NOMINMAX)
target_compile_definitions(parent PRIVATE NOMINMAX)
