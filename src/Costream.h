#pragma once
#include <string>
#include "WinConsoles.h"
#include "Win32Helpers.h"


namespace WinConsoles {
    class Cout {
    public:
        Cout(Win32Helpers::Hndl& writePipeHndl);
        void write(const std::string& content);
    private:
        Cout(const Cout&) = delete;
        Cout(Cout&&) = delete;

        Win32Helpers::Hndl m_writePipeHndl;

    };
};

