cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(console console.cpp Win32Helpers.h Win32Helpers.cpp)

if(MSVC)
    target_compile_options(console BEFORE PRIVATE /W4 /WX)
else()
    target_compile_options(console BEFORE PRIVATE -Wall -Werror -pedantic -Wextra)
endif()

target_compile_definitions(console PRIVATE NOMINMAX)
