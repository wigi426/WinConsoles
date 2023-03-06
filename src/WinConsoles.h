#pragma once
#include <string>
#include <memory>

namespace WinConsoles
{
    class Console_Impl;

    class Console {
    public:
        Console(
            const std::string& name = "WinConsoles Extra Console",
            int sizeX = 100,
            int sizeY = 100,
            int posX = 0,
            int posY = 0,
            bool bAutoClose = true);
        void write(const std::string& buffer);
        void read(std::string& buffer, const std::streamsize count, const char delim);
        void closeConsole();

        ~Console();
        Console(Console&&);
    private:
        Console(const Console&) = delete;
        std::unique_ptr<Console_Impl> pImpl;
    };
};