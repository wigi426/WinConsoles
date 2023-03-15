#pragma once
#include <Windows.h>
#include <memory>

namespace Win32Helpers
{
    class Hndl {
    public:
        Hndl(HANDLE hndl);
        Hndl(const Hndl& original);
        Hndl(Hndl&& original);
        ~Hndl();
        HANDLE& get();
    private:
        std::shared_ptr<HANDLE> m_hndl;
        bool m_bValid{ true };
    };
};