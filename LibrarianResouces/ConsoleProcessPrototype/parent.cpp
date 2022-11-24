#include <iostream>
#include <string>
#include <thread>
#include <limits>
#include <Windows.h>
#include <chrono>
#include <Win32Toolbelt.h>



int main()
{
    HANDLE inReadPipe{ nullptr };
    HANDLE inWritePipe{ nullptr };
    HANDLE outReadPipe{ nullptr };
    HANDLE outWritePipe{ nullptr };
    HANDLE cmdWritePipe{ nullptr };
    HANDLE cmdReadPipe{ nullptr };


    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.bInheritHandle = true;
    sa.lpSecurityDescriptor = NULL;
    sa.nLength = sizeof(sa);
    CreatePipe(&inReadPipe, &inWritePipe, &sa, 0);
    CreatePipe(&outReadPipe, &outWritePipe, &sa, 0);
    CreatePipe(&cmdReadPipe, &cmdWritePipe, &sa, 0);

    SetHandleInformation(outWritePipe, HANDLE_FLAG_INHERIT, FALSE);
    SetHandleInformation(inReadPipe, HANDLE_FLAG_INHERIT, FALSE);
    SetHandleInformation(cmdWritePipe, HANDLE_FLAG_INHERIT, FALSE);


    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.hStdInput = outReadPipe;
    si.hStdOutput = inWritePipe;
    si.hStdError = NULL;
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    std::string cmdLine{ std::to_string(reinterpret_cast<intptr_t>(cmdReadPipe)) };
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

    WinHANDLE_stdStreamAssociation<std::ostream, std::ofstream> childConOut(outWritePipe);
    WinHANDLE_stdStreamAssociation<std::istream, std::ifstream> childConIn(inReadPipe);
    WinHANDLE_stdStreamAssociation<std::ostream, std::ofstream> childCmd(cmdWritePipe);


    childCmd.get().write("r\n", 2);
    childCmd.get().flush();

    childConOut.get() << "hello World" << std::endl;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "requesting input from child console" << std::endl;
    childCmd.get().write("w", 1);
    childCmd.get().flush();
*    std::string rBuff{};
    rBuff.resize(500);
    childConIn.get().read(rBuff.data(), rBuff.size());
    std::cout << "got input from child console: " << rBuff << std::endl;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


    childCmd.get().write("e", 1);
    childCmd.get().flush();


    CloseHandle(cmdReadPipe);
    CloseHandle(outReadPipe);
    CloseHandle(inWritePipe);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}
