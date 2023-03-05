#pragma once
#include <string>
#include <Windows.h>
#include "WinConsoles.h"
#include "Win32Helpers.h"

class Cout {
public:
    Cout(Win32Helpers::Hndl writePipeHndl, Win32Helpers::Hndl cmdPipeHndl)
private:
    Cout(const Cout&) = delete;
    Cout(Cout&&) = delete;

    Win32Helpers::Hndl m_writePipeHndl;
    Win32Helpers::Hndl m_cmdPipeHndl;

    void write(const std::string& content);
};

