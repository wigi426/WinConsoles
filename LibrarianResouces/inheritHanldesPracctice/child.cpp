#include <iostream>
#include <sstream>
#include <Windows.h>

int main(int argc, char* argv[])
{
    for (int i{}; i < argc; i++)
    {
        std::cout << argv[i] << std::endl;
    }

    std::string readHandle{ argv[0] };
    std::uintptr_t readIntPtr{ std::stoull(readHandle) };
    HANDLE read = reinterpret_cast<HANDLE>(readIntPtr);

    char buff[100];
    std::cout << readIntPtr << std::endl;
    ReadFile(read, buff, 100, NULL, NULL);
    std::cout << buff << std::endl;

    return 0;
}