#include <iostream>
#include <string>
#include <future>
#include <cassert>
#include <fstream>
#include <io.h>
#include <Windows.h>

void pollCmdInStream(HANDLE cmdInStream, std::ostream& out)
{
    bool bExit{ false };
    const int16_t buffSize{ 5000 };
    const int8_t cmdBuffSize{ 10 };
    char outBuff[buffSize] = { '\0' };
    char cmdBuff[cmdBuffSize] = { '\0' };
    do {


        DWORD bytesRead;
        if (!ReadFile(cmdInStream, cmdBuff, cmdBuffSize, &bytesRead, NULL))
        {
            std::cout << "issue reading from read pipe, press enter to close console...\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return;
        }
        std::cout << "read" << bytesRead << std::endl;

        if (bytesRead)
        {
            if (!std::strcmp(cmdBuff, "exit"))
                bExit = true;
            else if (!std::strcmp(cmdBuff, "read"))
            {
                // std::cin.read(outBuff.data(), outBuff.max_size());
                std::cin >> outBuff;
                out.write(outBuff, buffSize);;
            }
        }
    } while (!bExit);
    std::cout << "leaving poll func";
    return;
}

int main(int argc, char* argv[]) {


    std::istream in(std::cin.rdbuf());//pass along inherited streams
    std::ostream out(std::cout.rdbuf());//pass along inherited streams
    //AllocConsole();
    //make std::cout and std::cin associated with the console
    HANDLE HCOUT{ CreateFileA("CONOUT$", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL) };
    assert(HCOUT != INVALID_HANDLE_VALUE);
    HANDLE HCIN{ CreateFileA("CONIN$", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL) };
    assert(HCIN != INVALID_HANDLE_VALUE);

    int cinFD = _open_osfhandle(reinterpret_cast<intptr_t>(HCIN), 0);
    assert(cinFD != -1 && "could not create file descriptor from windows file handle");
    FILE* cinFile = _fdopen(cinFD, "r");
    assert(cinFile != nullptr && "could not open file from file descriptor");
    std::ofstream fcin(cinFile);
    assert(fcin.good());
    std::cin.rdbuf(fcin.rdbuf());
    assert(std::cin.good());

    int coutFD = _open_osfhandle(reinterpret_cast<intptr_t>(HCOUT), 0);
    assert(coutFD != -1 && "could not create file descriptor from windows file handle");
    FILE* coutFile = _fdopen(coutFD, "a");
    assert(coutFile != nullptr && "could not open file from file descriptor");
    std::ofstream fcout(coutFile);
    assert(fcout.good());
    std::cout.rdbuf(fcout.rdbuf());
    assert(std::cout.good());





    for (int i{}; i < argc; i++)
    {
        std::cout << argv[i] << '\n';
    }

    std::cout << sizeof(long);
    std::cout << "hello world 12345";
    out << "yo from the console\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');



    //prescribed arg count
    if (argc != 2)
    {
        std::cout << "console was started with incorrect number of arguments, press enter to close console...\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return 1;
    }

#define CONVERSION_TO_GREATER_SIZE 4312
    //get value for passed in command pipe
    HANDLE commandReadPipe = nullptr;
#pragma warning(push)
#pragma warning(disable: CONVERSION_TO_GREATER_SIZE)
    if (!std::strcmp(argv[0], std::to_string(sizeof(long)).c_str()))
    {
        commandReadPipe = reinterpret_cast<HANDLE>(std::stol(argv[1]));
    }
    else if (!std::strcmp(argv[0], std::to_string(sizeof(long long)).c_str()))
    {
        commandReadPipe = reinterpret_cast<HANDLE>(std::stoll(argv[1]));
    }
    else
    {
        assert("intptr_t is of unexpected size(not 4 or 8 bytes)");
    }
#pragma warning(pop)
#undef CONVERSION_TO_GREATER_SIZE

    auto cmdPollFuture = std::async(std::launch::async, [&]() {return pollCmdInStream(commandReadPipe, out);});

    bool bExit{ false };
    //const uint16_t buffSize{ 5000 };
    //char inBuff[buffSize] = { '\0' };
    do {
        //  in.read(inBuff.data(), inBuff.max_size());

        //in.read(inBuff, buffSize);
        //if (in.gcount() > 0)
          //  std::cout << inBuff;
        if (cmdPollFuture._Is_ready())
        {
            cmdPollFuture.get();
            bExit = true;
        }
    } while (!bExit);
    std::cout << "closing?" << std::endl;
    fclose(coutFile);
    fclose(cinFile);
    fcin.close();
    fcout.close();
    return 0;
}