#pragma once
#include <Windows.h>

namespace Win32Helpers
{
    class Hndl {
    public:
        Hndl(HANDLE hndl);
        ~Hndl();
        HANDLE& get();
    private:
        HANDLE m_hndl;
    }

};