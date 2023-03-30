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
        struct HANDLE_SHARED {
            HANDLE_SHARED(HANDLE hndl);
            HANDLE_SHARED(HANDLE_SHARED&& original);
            ~HANDLE_SHARED();
            HANDLE m_rawHandle;
            bool m_bValid{ true };
        private:
            HANDLE_SHARED(const HANDLE_SHARED& original) = delete;
        };
        std::shared_ptr<HANDLE_SHARED> m_hndl;
    };
};