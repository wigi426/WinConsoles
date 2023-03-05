#include "Win32Helpers.h"

namespace Win32Helpers {
#pragma region Hndl
    Hndl::Hndl(HANDLE hndl): m_hndl{ std::make_shared<HANDLE>(hndl) } {}
    Hndl::Hndl(const Hndl& original): m_hndl{ original.m_hndl } {}
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