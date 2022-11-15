#include <iostream>
#include <string>
#include <thread>
#include <limits>
#include <Windows.h>

int main()
{
    HANDLE inReadPipe{ nullptr };
    HANDLE inWritePipe{ nullptr };
    HANDLE outReadPipe{ nullptr };
    HANDLE outWritePipe{ nullptr };
    HANDLE cmdOutReadPipe{ nullptr };
    HANDLE cmdOutWritePipe{ nullptr };

    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.bInheritHandle = true;
    sa.lpSecurityDescriptor = NULL;
    sa.nLength = sizeof(sa);
    CreatePipe(&inReadPipe, &inWritePipe, &sa, 0);
    CreatePipe(&outReadPipe, &outWritePipe, &sa, 0);
    CreatePipe(&cmdOutReadPipe, &cmdOutWritePipe, &sa, 0);
    SetHandleInformation(outWritePipe, HANDLE_FLAG_INHERIT, FALSE);
    SetHandleInformation(inReadPipe, HANDLE_FLAG_INHERIT, FALSE);
    SetHandleInformation(cmdOutWritePipe, HANDLE_FLAG_INHERIT, FALSE);


    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.hStdInput = outReadPipe;
    si.hStdOutput = inWritePipe;
    si.hStdError = NULL;
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    std::string cmdLine{ std::to_string(sizeof(intptr_t)).append(" ") };
    cmdLine.append(std::to_string(reinterpret_cast<intptr_t>(cmdOutReadPipe)));
    std::cout << cmdLine << '\n';
    CreateProcessA("ConsoleProcess.exe",
        cmdLine.data(),
        NULL,
        NULL,
        TRUE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si,
        &pi);
    WaitForInputIdle(pi.hProcess, INFINITE);
    char rbuff[100]{};
    std::string cmdBuff{ "read" };
    WriteFile(cmdOutWritePipe, cmdBuff.c_str(), static_cast<DWORD>(cmdBuff.size()), NULL, NULL);
    ReadFile(inReadPipe, rbuff, 100, NULL, NULL);
    std::cout << rbuff << '\n';
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string buff{ "exit" };
    DWORD bytesToWrite{};
    if (buff.size() > std::numeric_limits<DWORD>::max())
    {
        bytesToWrite = std::numeric_limits<DWORD>::max();
    }
    else
    {
        bytesToWrite = static_cast<DWORD>(buff.size());
    }
    WriteFile(cmdOutWritePipe, buff.c_str(), bytesToWrite, NULL, NULL);
    std::cout << "just tried to close extra console\n";
    WaitForSingleObject(pi.hProcess, INFINITE);
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return 0;
}
