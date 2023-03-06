#include "WinConsoles.h"
#include "Console.h"
#include "Costream.h"
#include "Cistream.h"

//connecting front end interface to implementations
namespace WinConsoles
{
    Console::Console(const std::string& name,
        int sizeX,
        int sizeY,
        int posX,
        int posY,
        bool bAutoClose)
    {
        pImpl = std::make_unique<Console_Impl>(name, sizeX, sizeY, posX, posY, bAutoClose);
    }

    void Console::write(const std::string& buffer)
    {
        pImpl.get()->write(buffer);
    }

    void Console::read(std::string& buffer, const std::streamsize count, const char delim)
    {
        pImpl.get()->read(buffer, count, delim);
    }


    void Console::closeConsole()
    {
        pImpl.get()->closeConsole();
    }

    Console::~Console()
    {

    }
};

