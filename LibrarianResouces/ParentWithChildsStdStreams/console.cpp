#include <iostream>
#include <string>
#include <mutex>
#include <thread>
#include <chrono>
#include <future>
#include <cassert>
#include <fstream>
#include <Windows.h>
#include <Win32Toolbelt.h>

class exit {
public:
    inline static bool bExit{ false };
    inline static std::mutex bExit_mutex;
};

//waits for the parent process to close,
//therefore if the parent is not able to send the exit command to the in pipe
//then we still close this console on parent close
void waitForParentClose(HANDLE hParentProc) {
    WaitForSingleObject(hParentProc, INFINITE);
    const std::lock_guard<std::mutex> lock(exit::bExit_mutex);
    exit::bExit = true;
}

void pollForExitCommand()
{
    bool bExit{ false };
    do {
        std::string inCmd{};
        char c = static_cast<char>(std::cin.peek());
        if (c == std::istream::traits_type::eof())
            assert(false && "std::cin reached eof");
        const std::lock_guard<std::mutex> lock(exit::bExit_mutex);
        if (exit::bExit)
        {
            bExit = true;
        }
        else
        {
            exit::bExit = true;
            bExit = true;
        }
    } while (!bExit);
}


int main(int argc, char* argv[])
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    //########## check arg count 

    if (argc != 1) {
        std::runtime_error("incorrect number of args passed into ConsoleProcess");
    }


    //########## get parent process ID from args and open handle to parent

    HANDLE hParentProc = OpenProcess(PROCESS_DUP_HANDLE | SYNCHRONIZE, false, std::stoul(argv[0]));
    if (hParentProc == NULL)
    {
        std::runtime_error("could no get handle to parent process");
    }

    //########## acquire console stream HANDLES

    HANDLE hConOut{ CreateFile("CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL) };
    if (hConOut == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Could not open HANDLE to console out stream");
    HANDLE hConIn{ CreateFile("CONIN$", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL) };
    if (hConIn == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Could not open HANDLE to console in stream");

    //########## duplicate Hanlde

    HANDLE hParentDupConOut{ nullptr };
    HANDLE hParentDupConIn{ nullptr };
    if (!DuplicateHandle(GetCurrentProcess(), hConOut, hParentProc, &hParentDupConOut, 0, false, DUPLICATE_SAME_ACCESS))
        std::runtime_error("failed to duplicate Console Out Handle");
    if (!DuplicateHandle(GetCurrentProcess(), hConIn, hParentProc, &hParentDupConIn, 0, false, DUPLICATE_SAME_ACCESS))
        std::runtime_error("failed to duplicate Console Out Handle");


    //########## send them formatted into ints up the std::cout stream

    //stream data formatted as follows:
    //"iHANDLE;oHANDLE"
    //might look like:
    //"d234;186"

    std::string streamData(
        std::to_string(reinterpret_cast<intptr_t>(hParentDupConIn)) +
        ";" +
        std::to_string(reinterpret_cast<intptr_t>(hParentDupConOut)) +
        "\n"
    );

    std::cout << streamData;
    std::cout.flush();

    //########## wait for exit
    waitForParentClose(hParentProc);


    std::string inCmd{};
    char c = static_cast<char>(std::cin.peek());
    if (c == std::istream::traits_type::eof())
        assert(false && "std::cin reached eof");

    CloseHandle(hParentProc);
    CloseHandle(hConOut);
    CloseHandle(hConIn);

}