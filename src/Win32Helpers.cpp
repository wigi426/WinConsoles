#include "Win32Helpers.h"

namespace Win32Helpers {
#pragma region Hndl
    Hndl::Hndl(HANDLE hndl): m_hndl{ std::make_shared<HANDLE_SHARED>(hndl) } {}
    Hndl::Hndl(const Hndl& original): m_hndl{ original.m_hndl } {}
    Hndl::Hndl(Hndl&& original): m_hndl{ std::move(original.m_hndl) } {}
    Hndl::~Hndl() {}
    HANDLE& Hndl::get()
    {
        return m_hndl.get()->m_rawHandle;
    }
    Hndl::HANDLE_SHARED::HANDLE_SHARED(HANDLE hndl): m_rawHandle{ hndl } {}
    Hndl::HANDLE_SHARED::HANDLE_SHARED(HANDLE_SHARED&& original): m_rawHandle{ original.m_rawHandle }, m_bValid{ original.m_bValid }
    {
        original.m_bValid = false;
    }
    Hndl::HANDLE_SHARED::~HANDLE_SHARED()
    {
        if (m_bValid)
        {
            DWORD hndlInfoFlags;
            if (GetHandleInformation(m_rawHandle, &hndlInfoFlags))
            {
                if (!(hndlInfoFlags & HANDLE_FLAG_PROTECT_FROM_CLOSE))
                    CloseHandle(m_rawHandle);
            }
            m_bValid = false;
        }
    }
#pragma endregion
};