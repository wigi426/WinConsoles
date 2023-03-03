#include "Console.h"
#include "Win32Helpers.h"

Console_Impl::Console_Impl(const std::string& name, int sizeX, int sizeY, int posX, int posY, bool bAutoClose)
{
    // create the pipes
    Win32Helpers::Hndl cmdPipeIn{ nullptr };
    Win32Helpers::Hndl cmdPipeOut{ nullptr };
    Win32Helpers::Hndl writeConsolePipeOut{ nullptr };
    Win32Helpers::Hndl writeConsolePipeIn{ nullptr };
    Win32Helpers::Hndl readConsolePipeOut{ nullptr };
    Win32Helpers::Hndl readConsolePipeIn{ nullptr };

    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    sa.nLength = sizeof(sa);
    if (!CreatePipe(&(cmdPipeIn.get()), &(cmdPipeOut.get()), &sa, 0) ||
        !CreatePipe(&(writeConsolePipeIn.get()), &(writeConsolePipeOut.get()), &sa, 0) ||
        !CreatePipe(&(readConsolePipeIn.get()), &(readConsolePipeOut.get()), &sa, 0))
        throw std::runtime_error("Could not create win32 annonymous pipes");

    // configure pipes
    if (!SetHandleInformation(cmdPipeOut.get(), HANDLE_FLAG_INHERIT, FALSE) ||
        !SetHandleInformation(writeConsolePipeOut.get(), HANDLE_FLAG_INHERIT, FALSE) ||
        !SetHandleInformation(readConsolePipeIn.get(), HANDLE_FLAG_INHERIT, FALSE))
        throw std::runtime_error("Could not configure pipes attributes");

    // construct cmd line args for console program to pass it the pipes
    // add posiiton and size arguments for console window
    std::string cmdLine{ std::to_string(reinterpret_cast<intptr_t>(cmdPipeIn.get())) + " " +
        std::to_string(reinterpret_cast<intptr_t>(writeConsolePipeIn.get())) + " " +
        std::to_string(reinterpret_cast<intptr_t>(readConsolePipeOut.get())) };

    // create the console process
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;

    if (!CreateProcessA("winConsoles_console.exe",
        cmdLine.data(),
        NULL,
        NULL,
        TRUE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si,
        &pi))
        throw std::runtime_error("could not open winConsoles_console.exe");
    // wait for idle input
    if (WaitForInputIdle(pi.hProcess, 2000) != 0)
    {
        throw std::runtime_error("winConsoles_console.exe did not open properly");
    }

    // construct pipe objects local to this class
}