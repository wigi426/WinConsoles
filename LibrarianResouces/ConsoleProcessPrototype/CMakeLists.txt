
cmake_minimum_required(VERSION 3.1)

project(ConsoleProcess)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_BUILD_TYPE Debug) # set according to lib setting

add_executable(ConsoleProcess console.cpp)
add_executable(parent parent.cpp)

target_compile_definitions(ConsoleProcess PRIVATE NOMINMAX)
target_compile_definitions(parent PRIVATE NOMINMAX)

if(MSVC)
    target_compile_options(ConsoleProcess PUBLIC /W4 /WX)
    target_compile_options(parent PUBLIC /W4 /WX)
else()
    target_compile_options(ConsoleProcess PUBLIC /Wall /Wpedantic /Wextra /Werror)
    target_compile_options(parent PUBLIC /Wall /Wpedantic /Wextra /Werror)
endif()
