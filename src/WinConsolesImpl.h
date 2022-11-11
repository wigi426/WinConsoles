#pragma once
#include "WinConsoles.h"
#include <map>
#include <string>
#include <concepts>
#include <memory>
#include <sstream>
#include <cassert>
#include <Windows.h>
#include <processthreadsapi.h>
#include <namedpipeapi.h>


namespace WinConsoles
{

    // the value of N = what to treat as the invalid handle value
    // either NULL or INVALID_HANDLE_VALUE
    template <HANDLE INVALID_VAL>
    struct WinHANDLE
    {
        HANDLE handle{ INVALID_VAL };
        constexpr static HANDLE invalidVal{ INVALID_VAL };
        HANDLE* operator&() { return &handle; }
        ~WinHANDLE()
        {
            if (handle != INVALID_VAL)
                CloseHandle(handle);
        }
    };

    template <typename T>
        requires std::same_as<T, PROCESS_INFORMATION> ||
    std::same_as<T, STARTUPINFO> ||
        std::same_as<T, SECURITY_ATTRIBUTES>
        struct WinSTRUCT
    {
        T structure;
        T* operator->() { return &structure; } // how tf does this work
        T& operator*() { return structure; }
        T* operator&() { return &structure; }
        WinSTRUCT<T>() { ZeroMemory(&structure, sizeof(structure)); }
        ~WinSTRUCT<T>() {};
    };

    template <>
    WinSTRUCT<PROCESS_INFORMATION>::~WinSTRUCT();

    class ConsoleImpl
    {
    public:
        static bool CreateConsole(const CONSOLE_ID ID);
        static bool CloseConsole(const CONSOLE_ID ID);
        static CONSOLE_STREAMSIZE Write(const char* const buff, const CONSOLE_STREAMSIZE size, const CONSOLE_ID ID);
        static CONSOLE_STREAMSIZE Read(char* const buff, const CONSOLE_STREAMSIZE size, const CONSOLE_ID ID);

        ConsoleImpl(const CONSOLE_ID ID);
    private:
        struct PipeHandles;
        struct ConsoleProcess;

        static inline std::map<CONSOLE_ID, std::unique_ptr<ConsoleImpl>> sm_consoles{};



        bool Init();
        bool Close();
        CONSOLE_STREAMSIZE WriteToConsole(const char* const buff, const CONSOLE_STREAMSIZE size);
        CONSOLE_STREAMSIZE ReadFromConsole(char* const buff, const CONSOLE_STREAMSIZE size);

        struct PipeHandles
        {
            enum class PIPE_DIRECTION
            {
                O, // writing out
                I  // reading in
            };

            WinHANDLE<INVALID_HANDLE_VALUE> hPipeWrite{};
            WinHANDLE<INVALID_HANDLE_VALUE> hPipeRead{};
            bool bInit{ false };

            PipeHandles(const PIPE_DIRECTION& direction);

            PipeHandles() = delete;
            PipeHandles(const PipeHandles&) = delete;
            PipeHandles(PipeHandles&&) = delete;
        };

        struct ConsoleProcess
        {
            WinSTRUCT<PROCESS_INFORMATION> piLogger{};
            bool bInit{ false };

            ConsoleProcess(const PipeHandles& pipeOut, const PipeHandles& pipeIn);
            ~ConsoleProcess();

            ConsoleProcess() = delete;
            ConsoleProcess(const ConsoleProcess&) = delete;
            ConsoleProcess(ConsoleProcess&&) = delete;
        };

        bool m_bInit{ false };
        std::unique_ptr<PipeHandles> m_InPipe{ nullptr };
        std::unique_ptr<PipeHandles> m_OutPipe{ nullptr };
        CONSOLE_ID m_ID;


        ConsoleImpl() = delete;
        ConsoleImpl(const ConsoleImpl&) = delete;
        ConsoleImpl(ConsoleImpl&&) = delete;

        // Creates a Win32 message box with the last error recievved from windows with
        // GetLastError, contextMessage should give information on where the failure occured
        // message box blocks execution until ok button is pressed
        // macro copies line and file into function call
#if defined(WINCONSOLES_ERROR_BOXES)
#define CreateSystemErrorBox(contextMessage) CreateSystemErrorBoxW(contextMessage, __LINE__, __FILE__, true)
        // extra macro to call function without system error output, just a dialog box with ok button which will include the context message and file/line values
#define CreateErrorBox(contextMessage) CreateSystemErrorBoxW(contextMessage, __LINE__, __FILE__, false)
#else
#define CreateSystemErrorBox(contextMessage) d
#define CreateErrorBox(contextMessage)
#endif
        static void CreateSystemErrorBoxW(const std::wstring_view& contextMessage, const int line, const char* file, const bool systemErr = true);
    };
};