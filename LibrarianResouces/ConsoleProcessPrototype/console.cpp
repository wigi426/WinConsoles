#include <iostream>
#include <string>
#include <future>
#include <cassert>
#include <Windows.h>


int main(int argc, char* argv[]) {
    //prescribed arg count
    if (argc != 1)
    {
        std::cout << "console was started with incorrect number of arguments, press enter to close console...\n";
        std::cin.get();
        return 1;
    }

#define CONVERSION_TO_GREATER_SIZE 4312
    //get value for passed in command pipe
    HANDLE commandReadPipe = nullptr;
#pragma warning(push)
#pragma warning(disable: CONVERSION_TO_GREATER_SIZE)
    if constexpr (sizeof(long long) == sizeof(intptr_t))
    {
        commandReadPipe = reinterpret_cast<HANDLE>(std::stoll(argv[0]));
    }
    else if constexpr (sizeof(long) == sizeof(intptr_t))
    {
        commandReadPipe = reinterpret_cast<HANDLE>(std::stol(argv[0]));
    }
    else
    {
        assert("intptr_t is of unaccounted size");
    }
#pragma warning(pop)
#undef CONVERSION_TO_GREATER_SIZE

    //verify handle is pointing at valid object
    if (!GetHandleInformation(commandReadPipe, NULL))
    {
        std::cout << "invalid handle for command pipe passed to console, press enter to close console...\n";
        std::cin.get();
        return 1;
    }

    bool bExit{ false };
    const uint16_t buffSize{ 5000 };
    char inBuff[buffSize] = { '\0' };
    char outBuff[buffSize] = { '\0' };
    char cmdBuff[buffSize] = { '\0' };
    std::istream in(std::cin.rdbuf());//pass 5along inherited streams
    std::ostream out(std::cout.rdbuf());//pass along inherited streams
    if (!AllocConsole())
        std::cout << "alloc console failed, console for this process already exists\n";
    do {
        //  in.read(inBuff.data(), inBuff.max_size());
        in >> inBuff;
        std::cout << inBuff;
        DWORD bytesRead;
        if (!ReadFile(commandReadPipe, cmdBuff, buffSize, &bytesRead, NULL))
        {
            std::cout << "issue reading from read pipe, press enter to close console...\n";
            std::cin.get();
            return 1;
        }

        if (bytesRead)
        {
            if (std::strcmp(cmdBuff, "exit"))
                bExit = true;
            else if (std::strcmp(cmdBuff, "read"))
            {
                // std::cin.read(outBuff.data(), outBuff.max_size());
                std::cin >> outBuff;
                out << outBuff;
            }
        }
    } while (!bExit);
    return 0;
}