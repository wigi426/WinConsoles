#include "Win32Helpers.h"

namespace Win32Helpers {
#pragma region Hndl
    Hndl::Hndl(HANDLE hndl): m_hndl{ std::make_shared<HANDLE>(hndl) } {}
    Hndl::Hndl(const Hndl& original): m_hndl{ original.m_hndl } {}
    Hndl::Hndl(Hndl&& original): m_hndl{ std::move(original.m_hndl) } {
        original.m_bValid = false;
    }
    Hndl::~Hndl()
    {
        if (m_bValid)
            CloseHandle(*(m_hndl.get()));
    }
    HANDLE& Hndl::get()
    {
        return *(m_hndl.get());
    }
#pragma endregion
};