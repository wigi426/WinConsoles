#pragma once
#include <string>
#include "WinConsoles.h"
#include "Costream.h"
#include "Cistream.h"


class Console_Impl {
public:
    Console_Impl(
        const std::string& name,
        int sizeX,
        int sizeY,
        int posX,
        int posY,
        bool bAutoClose = true);
    WinConsoles::Costream& getOut();
    WinConsoles::Cistream& getIn();
    void closeConsole();

    Console_Impl(Console_Impl&&);
    ~Console_Impl() = default;
};
