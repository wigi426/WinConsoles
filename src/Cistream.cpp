#include "Cistream.h"
#include <Windows.h>
#include <stdexcept>

namespace WinConsoles {

    Cin::Cin(Win32Helpers::Hndl& readPipeHndl, Win32Helpers::Hndl& cmdPipeHndl):
        m_readPipeHndl{ readPipeHndl }, m_cmdPipeHndl{ cmdPipeHndl } {}
    //TODO: add a storeDelim bool parameter, which will allow the user to choose whether the delimiter is included in the string given back to them
    void Cin::read(std::string& buffer, const std::streamsize count, const char delim, const bool storeDelim)
    {
        //send cmd to console to read
        std::string delimStr;
        if (delim == '\n')
            delimStr = "\\n";
        else
            delimStr = delim;
        std::string readCmdMsg{"r;" + std::to_string(count) + ";" + delimStr + ";1;\n"};
        if (!WriteFile(m_cmdPipeHndl.get(), readCmdMsg.c_str(), static_cast<DWORD>(readCmdMsg.size()), NULL, NULL))
            throw std::runtime_error("Could not write to cmd pipe");

        //retrieve read data or timeout and throw.
        if (buffer.size() < static_cast<size_t>(count))
            buffer.resize(count);
        static DWORD bytesRead;
        if (!ReadFile(m_readPipeHndl.get(), buffer.data(), static_cast<DWORD>(count), &bytesRead, NULL))
            throw std::runtime_error("Could not read from read pipe");
        if (bytesRead < count)
            buffer.resize(bytesRead);
        if (!storeDelim)
            if (buffer.at(bytesRead - 1) == delim)
                buffer.erase(bytesRead - 1);
    }

    ConfirmReceiver::ConfirmReceiver(Win32Helpers::Hndl& confirmPipeHndl): m_confirmPipeHndl{ confirmPipeHndl } {}

    void ConfirmReceiver::read(char& c)
    {
        char buffer[2]{};
        if (!ReadFile(m_confirmPipeHndl.get(), buffer, 1, NULL, NULL))
            throw std::runtime_error("Could not read from Confirm Pipe");
        c = buffer[0];
    }
};