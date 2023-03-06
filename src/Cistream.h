#pragma once
#include <string>
#include <Windows.h>
#include "WinConsoles.h"
#include "Win32Helpers.h"

namespace WinConsoles {
    class Cin {
    public:
        Cin(Win32Helpers::Hndl& readPipeHndl, Win32Helpers::Hndl& cmdPipeHndl);
    private:
        Cin(const Cin&) = delete;
        Cin(Cin&&) = delete;

        Win32Helpers::Hndl m_readPipeHndl;
        Win32Helpers::Hndl m_cmdPipeHndl;

        void read(std::string& buffer, std::streamsize count, char delim);
    };
};