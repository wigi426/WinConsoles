#include "Cistream.h"

namespace WinConsoles {
#pragma warning (push)
#pragma warning (disable : 4100)
    Cin::Cin(Win32Helpers::Hndl& readPipeHndl, Win32Helpers::Hndl& cmdPipeHndl):
        m_readPipeHndl{ readPipeHndl }, m_cmdPipeHndl{ cmdPipeHndl } {}

    void Cin::read(std::string& buffer, std::streamsize count, char delim)
    {
        //send cmd to console to read
        //retrieve read data or timeout and throw.
    }
#pragma warning (pop)
};