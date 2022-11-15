#include <iostream>
#include <future>
#include <string>
#include <Windows.h>

void someFunc(HANDLE cmdInStream, std::ostream& out)
{
    bool bExit{ false };
    const int16_t buffSize{ 5000 };
    const int8_t cmdBuffSize{ 10 };
    char outBuff[buffSize] = { '\0' };
    char cmdBuff[cmdBuffSize] = { '\0' };
    DWORD bytesRead{};
    do {
        ReadFile(cmdInStream, cmdBuff, cmdBuffSize, &bytesRead, NULL);
        if (bytesRead)
        {
            if (std::string(cmdBuff).find("exit") != std::string::npos)
                bExit = true;
            else if (std::string(cmdBuff).find("read") != std::string::npos)
            {
                // std::cin.read(outBuff.data(), outBuff.max_size());
                std::cin >> outBuff;
                out.write(outBuff, buffSize);;
            }
            std::cout << "read:" << bytesRead << ':' << cmdBuff;
        }
    } while (!bExit);
    return;
}

int main()
{
    std::ostream consoleOut(std::cout.rdbuf());
    HANDLE fileHndl = CreateFileA("CONIN$", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    auto future = std::async(std::launch::async, &someFunc, fileHndl, std::ref(consoleOut));


    future.wait();
    CloseHandle(fileHndl);
    return 0;
}