#pragma once
#include <string>
#include "WinConsoles.h"
#include "Costream.h"
#include "Cistream.h"


class Console_Impl {
protected:
    Console_Impl(
        const std::string& name,
        int sizeX,
        int sizeY,
        int posX,
        int posY,
        bool bAutoClose = true);
    friend class Console;
public:
    void write(const std::string& buffer);
    void read(std::string& buffer, std::streamsize count, char delim);
    void closeConsole();

    Console_Impl(Console_Impl&&);
    ~Console_Impl();

private:
    Cin cin;
    Cout cout;
};
