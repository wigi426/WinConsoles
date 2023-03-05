#pragma once
#include <Windows.h>
#include <memory>

namespace Win32Helpers
{

    class Hndl {
    public:
        Hndl(HANDLE hndl);
        Hndl(const Hndl& original);
        ~Hndl();
        HANDLE& get();
    private:
        Hndl(Hndl&&) = delete;
        std::shared_ptr<HANDLE> m_hndl;
    }

};