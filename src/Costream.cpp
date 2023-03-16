#include "Costream.h"
#include <stdexcept>
#include <Windows.h>

namespace WinConsoles {
#pragma warning (push)
#pragma warning (disable: 4100)
    Cout::Cout(Win32Helpers::Hndl& writePipeHndl):
        m_writePipeHndl{ writePipeHndl } {}

    void Cout::write(const std::string& content)
    {
        if (!WriteFile(m_writePipeHndl.get(), content.c_str(), static_cast<DWORD>(content.size()), NULL, NULL))
            throw std::runtime_error("failed to write to write pipe");
    }
#pragma warning(pop)
};