#include <iostream>
#include <Windows.h>
#include <processthreadsapi.h>
#include <chrono>


int main()
{

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;

    CreateProcess("dummy.exe",
        NULL,
        NULL,
        NULL,
        false,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si,
        &pi);

    WaitForInputIdle(pi.hProcess, INFINITE);




    FreeConsole();

    AttachConsole(pi.dwProcessId);

    std::cout << "hello World" << std::endl;

    std::cin.ignore(10000, '\n');

    return 0;
}