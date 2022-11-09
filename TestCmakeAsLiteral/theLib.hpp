#include <string>
#include <fstream>
#include <filesystem>
#include <cassert>
#include <Windows.h>
#include <processthreadsapi.h>

namespace theLib
{
    inline static const std::string CMAKE_LISTS_CONTENT{
        R"DELIM(
cmake_minimum_required(VERSION 3.10)

project(someProg)

add_executable(someProg "someProg.cpp")
        )DELIM"};

    inline static const std::string SOME_PROG_CPP_CONTENT{
        R"DELIM(
#include <iostream>

int main()
{
    std::cout << "Hello World from Some Prog" << std::endl;
    std::cin.get();
    return 0;
} 

        )DELIM"};

    void BuildProgram();
    void RunProgram();

};
