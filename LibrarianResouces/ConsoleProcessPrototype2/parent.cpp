#include <iostream>
#include <string>
#include <cassert>
#include <Windows.h>
#include <processthreadsapi.h>
#include <namedpipeapi.h>

int main()
{

    HANDLE cmdPipe_Write{ nullptr };
    HANDLE cmdPipe_Read{ nullptr };
    HANDLE consoleWritePipe_Write{ nullptr };
    HANDLE consoleWritePipe_Read{ nullptr };
    HANDLE consoleReadPipe_Write{ nullptr };
    HANDLE consoleReadPipe_Read{ nullptr };

    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.bInheritHandle = true;
    sa.lpSecurityDescriptor = NULL;
    sa.nLength = sizeof(sa);
    assert(CreatePipe(&cmdPipe_Read, &cmdPipe_Write, &sa, 0));
    assert(SetHandleInformation(cmdPipe_Write, HANDLE_FLAG_INHERIT, FALSE));
    assert(CreatePipe(&consoleWritePipe_Read, &consoleWritePipe_Write, &sa, 0));
    assert(SetHandleInformation(consoleWritePipe_Write, HANDLE_FLAG_INHERIT, FALSE));
    assert(CreatePipe(&consoleReadPipe_Read, &consoleReadPipe_Write, &sa, 0));
    assert(SetHandleInformation(consoleReadPipe_Read, HANDLE_FLAG_INHERIT, FALSE));

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    std::string cmdLine{};
    cmdLine.append(std::to_string(reinterpret_cast<intptr_t>(cmdPipe_Read)) + " ");
    cmdLine.append(std::to_string(reinterpret_cast<intptr_t>(consoleWritePipe_Read)) + " ");
    cmdLine.append(std::to_string(reinterpret_cast<intptr_t>(consoleReadPipe_Write)));
    assert(CreateProcessA(
        "ConsoleProcess.exe",
        cmdLine.data(),
        NULL,
        NULL,
        TRUE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si,
        &pi
    ));
    WaitForInputIdle(pi.hProcess, INFINITE);

    std::string cmd{ "w\n" };
    assert(WriteFile(cmdPipe_Write, cmd.data(), static_cast<DWORD>(cmd.size()), NULL, NULL));
    std::string logMsg{ "Hello World! from parent process!\r\n" };
    assert(WriteFile(consoleWritePipe_Write, logMsg.data(), static_cast<DWORD>(logMsg.size()), NULL, NULL));

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    cmd = "e";
    assert(WriteFile(cmdPipe_Write, cmd.data(), static_cast<DWORD>(cmd.size()), NULL, NULL));

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


    return 0;
}