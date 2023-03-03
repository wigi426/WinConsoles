#include "Win32Helpers.h"

namespace Win32Helpers {

#pragma region Hndl
    Hndl::Hndl(HANDLE hndl): m_hndl{ hndl } {}
    Hndl::~Hndl()
    {
        CloseHandle(m_hndl);
    }
    HANDLE& Hndl::get()
    {
        return m_hndl;
    }
#pragma endregion

};