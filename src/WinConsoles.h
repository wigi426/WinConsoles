#pragma once
#include <cstdint>

namespace WinConsoles
{
    class ConsoleImpl;
    using CONSOLE_ID = uint8_t;
    using CONSOLE_STREAMSIZE = uint32_t;
    class Console
    {
    public:
        // initalizes a Console, with an ID, this ID i used as a refrence
        // ID defaults to 0 as do functions which access
        static bool CreateConsole(const CONSOLE_ID ID);
        // same as calling CreateConsole(0), just defaults to console 0
        static bool CreateConsole();
        // destroys console window corresponding to ID
        static bool CloseConsole(const CONSOLE_ID ID);
        // same as calling CloseConsole(0), just defaults to console 0
        static bool CloseConsole();

        // writes to console corresponding to ID
        static CONSOLE_STREAMSIZE Write(const char* const buff, const CONSOLE_STREAMSIZE size, const CONSOLE_ID ID);
        // same as calling Write(buff, size, 0), just defaults to console 0
        static CONSOLE_STREAMSIZE Write(const char* const buff, const CONSOLE_STREAMSIZE size);

        // reads "size" bytes from console corresponding to ID
        static CONSOLE_STREAMSIZE Read(char* const buff, const CONSOLE_STREAMSIZE size, const CONSOLE_ID ID);
        // same as calling Read(buff, size, 0), just defaults to console 0
        static CONSOLE_STREAMSIZE Read(char* const buff, const CONSOLE_STREAMSIZE size);

    private:
        Console() = delete;
        Console(const Console&) = delete;
        Console(Console&&) = delete;
    };
};