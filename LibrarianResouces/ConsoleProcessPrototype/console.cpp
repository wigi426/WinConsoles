#include <iostream>
#include <string>
#include <future>
#include <cassert>
#include <Windows.h>


int main(int argc, char* argv[]) {
    std::cout << "hello3\n";
    std::istream in(std::cin.rdbuf());//pass along inherited streams
    std::ostream out(std::cout.rdbuf());//pass along inherited streams

    HANDLE cout{ CreateFileA("CONIN$", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL) };
    assert(cout != INVALID_HANDLE_VALUE);
    HANDLE cin{ CreateFileA("CONOUT$", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL) };
    assert(cin != INVALID_HANDLE_VALUE);

    //prescribed arg count
    char buff[100] = "hello world from new cout hanlde";
    WriteFile(cout, buff, 100, NULL, NULL);
    out << GetLastError();
    assert(0);
    std::ios_base::Init();
    if (argc != 2)
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
    if (std::strcmp(argv[0], std::to_string(sizeof(long)).c_str()))
    {
        commandReadPipe = reinterpret_cast<HANDLE>(std::stol(argv[1]));
    }
    else if (std::strcmp(argv[0], std::to_string(sizeof(long long)).c_str()))
    {
        commandReadPipe = reinterpret_cast<HANDLE>(std::stoll(argv[1]));
    }
    else
    {
        assert("intptr_t is of unexpected size(not 4 or 8 bytes)");
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

        std::cout << "read" << bytesRead << std::endl;

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