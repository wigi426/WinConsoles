#pragma once
#include <string>
#include "WinConsoles.h"
#include "Win32Helpers.h"

namespace WinConsoles {
    class Cin {
    public:
        Cin(Win32Helpers::Hndl& readPipeHndl, Win32Helpers::Hndl& cmdPipeHndl);
        void read(std::string& buffer, std::streamsize count, char delim);
    private:
        Cin(const Cin&) = delete;
        Cin(Cin&&) = delete;

        Win32Helpers::Hndl m_readPipeHndl;
        Win32Helpers::Hndl m_cmdPipeHndl;

    };

    class ConfirmReceiver {
    public:
        ConfirmReceiver(Win32Helpers::Hndl& confirmPipeHndl);
        void read(char& c);
    private:
        ConfirmReceiver(const ConfirmReceiver&) = delete;
        ConfirmReceiver(ConfirmReceiver&&) = delete;

        Win32Helpers::Hndl m_confirmPipeHndl;
    };
};