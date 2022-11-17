#include <iostream>
#include <string>
#include <sstream>
#include <Windows.h>
#include <Win32Toolbelt.h>

template<std::signed_integral T>
T stosi(const std::string& str)
{
    if constexpr (sizeof(T) == sizeof(int))
        return std::stoi(str);
    else if constexpr (sizeof(T) == sizeof(long))
        return std::stol(str);
    else if constexpr (sizeof(T) == sizeof(long long))
        return std::stoll(str);
    else
        return std::stoi(str);
}

int main()
{
    HANDLE inReadPipe{ nullptr };
    HANDLE inWritePipe{ nullptr };
    HANDLE outReadPipe{ nullptr };
    HANDLE outWritePipe{ nullptr };


    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.bInheritHandle = true;
    sa.lpSecurityDescriptor = NULL;
    sa.nLength = sizeof(sa);
    CreatePipe(&inReadPipe, &inWritePipe, &sa, 0);
    CreatePipe(&outReadPipe, &outWritePipe, &sa, 0);
    SetHandleInformation(outWritePipe, HANDLE_FLAG_INHERIT, FALSE);
    SetHandleInformation(inReadPipe, HANDLE_FLAG_INHERIT, FALSE);


    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.hStdInput = outReadPipe;
    si.hStdOutput = inWritePipe;
    si.hStdError = NULL;
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    std::string cmdLine{ std::to_string(GetCurrentProcessId()) };
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
    CloseHandle(outReadPipe);
    CloseHandle(inWritePipe);
    WinHANDLE_stdStreamAssociation<std::ostream, std::ofstream> consoleProcOut(outWritePipe);
    WinHANDLE_stdStreamAssociation<std::istream, std::ifstream> consoleProcIn(inReadPipe);
    std::string buff{};
    std::getline(consoleProcIn.get(), buff);
    buff.pop_back();
    std::string coninHndlValSubstr = buff.substr(0, buff.find_first_of(';'));
    std::string conoutHndlValSubstr = buff.substr(buff.find_first_of(';') + 1, buff.size() - buff.find_first_of(';'));

    WinHANDLE_stdStreamAssociation<std::istream, std::ifstream> ChildConIn(reinterpret_cast<HANDLE>(stosi<intptr_t>(coninHndlValSubstr)));
    WinHANDLE_stdStreamAssociation<std::ostream, std::ofstream> ChildConOut(reinterpret_cast<HANDLE>(stosi<intptr_t>(conoutHndlValSubstr)));

    ChildConOut.get().write("hello\n", 6);
    ChildConOut.get().flush();

    assert(ChildConOut.get().rdstate() != std::ios_base::badbit);
    assert(ChildConOut.get().rdstate() != std::ios_base::failbit);
    assert(ChildConOut.get().rdstate() != std::ios_base::eofbit);
    assert(ChildConOut.get().good());


    ChildConOut.get() << "Can i get some input please:\n";

    std::string input{};
    ChildConIn.get() >> input;

    ChildConOut.get() << "you input: " << input << '\n';


    std::cin.ignore(1000, '\n');

    std::cout << "attempting to close console with exit command\n";

    ChildConOut.~WinHANDLE_stdStreamAssociation();
    ChildConIn.~WinHANDLE_stdStreamAssociation();
    consoleProcOut.get() << "e\n";
    consoleProcOut.get().flush();


    std::cin.ignore(1000, '\n');

    return 0;
}