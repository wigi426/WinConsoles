#include <iostream>
#include <string>
#include <Windows.h>
#include <processthreadsapi.h>
#include <namedpipeapi.h>


int main() {
    HANDLE read;
    HANDLE write;
    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa, sizeof(sa));
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    sa.nLength = sizeof(sa);
    CreatePipe(&read, &write, &sa, 0);
    std::uintptr_t readIntPtr{ reinterpret_cast<std::uintptr_t>(read) };
    std::wstring readHandle{ std::to_wstring(readIntPtr) };
    SetHandleInformation(write, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    CreateProcess(
        L"child.exe",
        readHandle.data(),
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &si,
        &pi);


    char buff[100] = "HELLO WORLD!";
    WaitForInputIdle(pi.hProcess, INFINITE);
    WriteFile(write, &buff, 100, NULL, NULL);

    return 0;
}