#include <stdexcept>
#include <cstdlib>
#include <Windows.h>
#include "Console.h"
#include "Win32Helpers.h"
#include "ConsoleLoc.h"

namespace WinConsoles {

#pragma warning(push)
#pragma warning(disable : 4100)

    enum class POS_SIZE_ARG: unsigned long long {
        POS_X = 0,
        POS_Y = 1,
        SIZE_X = 2,
        SIZE_Y = 3,
        TOTAL,
    };

    Console_Impl::Console_Impl(const std::string& name, int sizeX, int sizeY, int posX, int posY, bool bAutoClose)
    {

        // create the pipes
        Win32Helpers::Hndl cmdPipeIn{ nullptr };
        Win32Helpers::Hndl cmdPipeOut{ nullptr };
        Win32Helpers::Hndl writeConsolePipeOut{ nullptr };
        Win32Helpers::Hndl writeConsolePipeIn{ nullptr };
        Win32Helpers::Hndl readConsolePipeOut{ nullptr };
        Win32Helpers::Hndl readConsolePipeIn{ nullptr };

        SECURITY_ATTRIBUTES sa;
        ZeroMemory(&sa, sizeof(sa));
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = NULL;
        sa.nLength = sizeof(sa);
        if (!CreatePipe(&(cmdPipeIn.get()), &(cmdPipeOut.get()), &sa, 0) ||
            !CreatePipe(&(writeConsolePipeIn.get()), &(writeConsolePipeOut.get()), &sa, 0) ||
            !CreatePipe(&(readConsolePipeIn.get()), &(readConsolePipeOut.get()), &sa, 0))
            throw std::runtime_error("Could not create win32 annonymous pipes");

        // configure pipes
        if (!SetHandleInformation(cmdPipeOut.get(), HANDLE_FLAG_INHERIT, FALSE) ||
            !SetHandleInformation(writeConsolePipeOut.get(), HANDLE_FLAG_INHERIT, FALSE) ||
            !SetHandleInformation(readConsolePipeIn.get(), HANDLE_FLAG_INHERIT, FALSE))
            throw std::runtime_error("Could not configure pipes attributes");



        //prepare positon and size args for cmd line
        int posSizeArgs[static_cast<int>(POS_SIZE_ARG::TOTAL)];
        posSizeArgs[static_cast<int>(POS_SIZE_ARG::POS_X)] = posX;
        posSizeArgs[static_cast<int>(POS_SIZE_ARG::POS_Y)] = posY;
        posSizeArgs[static_cast<int>(POS_SIZE_ARG::SIZE_X)] = sizeX;
        posSizeArgs[static_cast<int>(POS_SIZE_ARG::SIZE_Y)] = sizeY;

        std::string posSizeArgsStr[static_cast<int>(POS_SIZE_ARG::TOTAL)];

        //convert negative value to abosolute value and prepend number with 'n' to indicate to the console that the value 
        //should be interpreted as negative
        for (int i{}; i < static_cast<int>(POS_SIZE_ARG::TOTAL); ++i)
        {
            if (posSizeArgs[i] < 0)
            {
                posSizeArgsStr[i].append("n");
            }
            posSizeArgsStr[i].append(std::to_string(std::abs(posSizeArgs[i])));
        }

        // construct cmd line args for console program to pass it the pipes
        std::string cmdLine{ std::to_string(reinterpret_cast<intptr_t>(cmdPipeIn.get())) + " " +
            std::to_string(reinterpret_cast<intptr_t>(writeConsolePipeIn.get())) + " " +
            std::to_string(reinterpret_cast<intptr_t>(readConsolePipeOut.get())) + " " +
            posSizeArgsStr[static_cast<int>(POS_SIZE_ARG::SIZE_X)] + " " +
            posSizeArgsStr[static_cast<int>(POS_SIZE_ARG::SIZE_Y)] + " " +
            posSizeArgsStr[static_cast<int>(POS_SIZE_ARG::POS_X)] + " " +
            posSizeArgsStr[static_cast<int>(POS_SIZE_ARG::POS_Y)] };

        // create the console process
        STARTUPINFOA si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi;

        //get directory of console process

        std::string consolePath(CONSOLE_PATH);
        consolePath.append("/debug/console.exe");


        if (!CreateProcessA(consolePath.c_str(),
            cmdLine.data(),
            NULL,
            NULL,
            TRUE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si,
            &pi))
        {
            throw std::runtime_error("could not open winConsoles_console.exe GetLastError() val: " + std::to_string(GetLastError()));
        }

        // construct pipe objects local to this class
        cin = std::make_unique<Cin>(readConsolePipeIn, cmdPipeOut);
        cout = std::make_unique<Cout>(writeConsolePipeOut);

        std::string buf("", 1);
        ReadFile(readConsolePipeIn.get(), buf.data(), 1, NULL, NULL);
        if (buf.compare("c") != 0)
            throw std::runtime_error("winConsoles console did not open properly");
        // FIXME: currently all win32helpers::hndl objects are being deconstructed and all handles are being closed.
    }

    Console_Impl::~Console_Impl()
    {
        closeConsole();
    }

    void Console_Impl::write(const std::string& buffer)
    {
        cout.get()->write(buffer);
    }

    void Console_Impl::read(std::string& buffer, std::streamsize count, char delim)
    {
        cin.get()->read(buffer, count, delim);
    }

    void Console_Impl::closeConsole()
    {

    }
#pragma warning(pop)
};


