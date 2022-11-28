#include <iostream>
#include <string>
#include <cassert>
#include <Windows.h>
#include <processthreadsapi.h>
#include <namedpipeapi.h>
#include <Win32Toolbelt.h>

int main()
{
    HANDLE cmdPipeIn{ nullptr };
    HANDLE cmdPipeOut{ nullptr };
    HANDLE writeConsolePipeOut{ nullptr };
    HANDLE writeConsolePipeIn{ nullptr };
    HANDLE readConsolePipeOut{ nullptr };
    HANDLE readConsolePipeIn{ nullptr };

    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    sa.nLength = sizeof(sa);

    assert(CreatePipe(&cmdPipeIn, &cmdPipeOut, &sa, 0));
    assert(CreatePipe(&writeConsolePipeIn, &writeConsolePipeOut, &sa, 0));
    assert(CreatePipe(&readConsolePipeIn, &readConsolePipeOut, &sa, 0));

    assert(SetHandleInformation(cmdPipeOut, HANDLE_FLAG_INHERIT, FALSE));
    assert(SetHandleInformation(writeConsolePipeOut, HANDLE_FLAG_INHERIT, FALSE));
    assert(SetHandleInformation(readConsolePipeIn, HANDLE_FLAG_INHERIT, FALSE));

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;

    std::string cmdLine{ std::to_string(reinterpret_cast<intptr_t>(cmdPipeIn)) + " " +
            std::to_string(reinterpret_cast<intptr_t>(writeConsolePipeIn)) + " " +
            std::to_string(reinterpret_cast<intptr_t>(readConsolePipeOut)) };

    assert(CreateProcess("console.exe",
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

    CloseHandle(cmdPipeIn);
    CloseHandle(writeConsolePipeIn);
    CloseHandle(readConsolePipeOut);

    WinHANDLE_stdStreamAssociation<std::ostream, std::ofstream> cmdOut(cmdPipeOut);
    WinHANDLE_stdStreamAssociation<std::ostream, std::ofstream> writeConsoleOut(writeConsolePipeOut);
    WinHANDLE_stdStreamAssociation<std::istream, std::ifstream> readConsoleIn(readConsolePipeIn);

    writeConsoleOut.get() << "Hello world" << std::endl;

    writeConsoleOut.get().put('?');
    writeConsoleOut.get().flush();
    cmdOut.get() << 'e' << std::endl;

    std::cin.ignore(1000, '\n');


    return 0;
}