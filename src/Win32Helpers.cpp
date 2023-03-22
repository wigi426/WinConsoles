#include "Win32Helpers.h"

namespace Win32Helpers {
#pragma region Hndl
    Hndl::Hndl(HANDLE hndl): m_hndl{ std::make_shared<HANDLE>(hndl) } {}
    Hndl::Hndl(const Hndl& original): m_hndl{ original.m_hndl }, m_bValid{ original.m_bValid } {}
    Hndl::Hndl(Hndl&& original): m_hndl{ std::move(original.m_hndl) } {
        original.m_bValid = false;
    }
    Hndl::~Hndl()
    {
        //use GetHandleInformation() to verify that CloseHanlde is valid
        if (m_bValid)
        {
            DWORD hndlInfoFlags;
            if (GetHandleInformation(*(m_hndl.get()), &hndlInfoFlags))
            {
                if (!(hndlInfoFlags & HANDLE_FLAG_PROTECT_FROM_CLOSE))
                    CloseHandle(*(m_hndl.get()));
            }
        }
    }
    HANDLE& Hndl::get()
    {
        return *(m_hndl.get());
    }
#pragma endregion
};