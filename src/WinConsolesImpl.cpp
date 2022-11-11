#include "WinConsolesImpl.h"

namespace WinConsoles
{
#pragma region ConsoleImpl STATICS

#pragma warning(disable:4715) //del later
    bool ConsoleImpl::CreateConsole(const CONSOLE_ID ID)
    {
        if (sm_consoles.contains(ID))
        {
            // return true as console already exists with this ID
            return true;
        }
        sm_consoles.emplace(ID, std::make_unique<ConsoleImpl>(ID));
        if (!sm_consoles.at(ID).get()->Init())
        {

            CreateErrorBox(std::wstring(L"failed to initalize console ID: ") + std::to_wstring(ID));
            return false;
        }
    }

    bool ConsoleImpl::CloseConsole(const CONSOLE_ID ID)
    {
        if (!sm_consoles.at(ID).get()->Close())
        {
            CreateErrorBox(L"Failed to close Console");
            return false;
        }
        sm_consoles.erase(ID);
    }

    CONSOLE_STREAMSIZE ConsoleImpl::Write(const char* const buff, const CONSOLE_STREAMSIZE size, const CONSOLE_ID ID)
    {
        if (sm_consoles.contains(ID))
        {
            return sm_consoles.at(ID).get()->WriteToConsole(buff, size);
        }
        return 0;
    }

    CONSOLE_STREAMSIZE ConsoleImpl::Read(char* const buff, const CONSOLE_STREAMSIZE size, const CONSOLE_ID ID)
    {
        if (sm_consoles.contains(ID))
        {
            return sm_consoles.at(ID).get()->ReadFromConsole(buff, size);
        }
        return 0;
    }

    void ConsoleImpl::CreateSystemErrorBoxW(const std::wstring_view& contextMessage, const int line, const char* const file, const bool systemErr)
    {
        std::wstringstream fullMessage{};
        if (systemErr)
        {
            LPTSTR buff = NULL;
            DWORD sysErrCode = GetLastError();
            assert(FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                sysErrCode,
                0,
                (LPTSTR)&buff,
                0,
                NULL) &&
                "FormatMessageA() for System error in WinConsoles failed");
            fullMessage << "System Error in WinConsoles, context: \n\""
                << contextMessage << "\""
                << '\n'
                << "LINE: " << line << '\n'
                << "FILE: " << file << '\n'
                << "\nSystem Error: 0x"
                << std::hex
                << sysErrCode
                << ":\n"
                << buff;
        }
        else
        {
            fullMessage << "General Error in WinConsoles, context: \n\""
                << contextMessage << "\""
                << '\n'
                << "LINE: " << line << '\n'
                << "FILE: " << file;
        }
        MessageBox(NULL, fullMessage.str().c_str(), NULL, MB_OK);
    }
#pragma endregion

#pragma region ConsoleImpl NON - STATICS
    ConsoleImpl::ConsoleImpl(const CONSOLE_ID ID) : m_ID{ ID }
    {
    }

    bool ConsoleImpl::Init()
    {
        // create pipe
        // create logger
        return true;
    }

    bool ConsoleImpl::Close()
    {
        // send close command
        return true;
    }


#pragma warning(disable: 4100) //del this when start implementation
    CONSOLE_STREAMSIZE ConsoleImpl::WriteToConsole(const char* const buff, const CONSOLE_STREAMSIZE size)
    {
        // write to pipe
        return 0;
    }

    CONSOLE_STREAMSIZE ConsoleImpl::ReadFromConsole(char* const buff, const CONSOLE_STREAMSIZE size)
    {
        // read from pipe
        return 0;
    }

#pragma endregion

#pragma region PipeHanldes
    ConsoleImpl::PipeHandles::PipeHandles(const ConsoleImpl::PipeHandles::PIPE_DIRECTION& direction)
    {
        WinSTRUCT<SECURITY_ATTRIBUTES> sa;
        sa->bInheritHandle = true;
        sa->lpSecurityDescriptor = NULL;
        sa->nLength = sizeof(sa);
        if (!CreatePipe(&hPipeRead, &hPipeWrite, &sa, 0))
        {
            CreateSystemErrorBox(L"CreatePipe failed");
        }
    }
#pragma endregion

#pragma region ConsoleProcess

#pragma endregion

#pragma region WinSTUCT
    template <>
    WinSTRUCT<PROCESS_INFORMATION>::~WinSTRUCT()
    {
        if (structure.hProcess != INVALID_HANDLE_VALUE)
            CloseHandle(structure.hProcess);
        if (structure.hThread != INVALID_HANDLE_VALUE)
            CloseHandle(structure.hThread);
    }
#pragma endregion
}