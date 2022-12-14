#include "WinConsoles.h"
#include "WinConsolesImpl.h"

namespace WinConsoles
{
    bool Console::CreateConsole(const CONSOLE_ID ID)
    {
        return ConsoleImpl::CreateConsole(ID);
    }

    bool Console::CreateConsole()
    {
        return ConsoleImpl::CreateConsole(0);
    }

    bool Console::CloseConsole(const CONSOLE_ID ID)
    {
        return ConsoleImpl::CloseConsole(ID);
    }

    bool Console::CloseConsole()
    {
        return ConsoleImpl::CloseConsole(0);
    }

    CONSOLE_STREAMSIZE Console::Write(const char* const buff, const CONSOLE_STREAMSIZE size, const CONSOLE_ID ID)
    {
        return ConsoleImpl::Write(buff, size, ID);
    }

    CONSOLE_STREAMSIZE Console::Write(const char* const buff, const CONSOLE_STREAMSIZE size)
    {
        return ConsoleImpl::Write(buff, size, 0);
    }

    CONSOLE_STREAMSIZE Console::Read(char* const buff, const CONSOLE_STREAMSIZE size, const CONSOLE_ID ID)
    {
        return ConsoleImpl::Read(buff, size, ID);
    }

    CONSOLE_STREAMSIZE Console::Read(char* const buff, const CONSOLE_STREAMSIZE size)
    {
        return ConsoleImpl::Read(buff, size, 0);
    }




}

