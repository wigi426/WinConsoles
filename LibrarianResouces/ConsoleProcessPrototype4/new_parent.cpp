#include <Windows.h>
#include <cassert>
#include <string>
#include <stdexcept>
#include <iostream>

enum class POS_SIZE_ARG: unsigned long long {
    POS_X = 0,
    POS_Y = 1,
    SIZE_X = 2,
    SIZE_Y = 3,
    TOTAL,
};

int main()
{
    try {
        HANDLE cmdPipeIn{ nullptr };
        HANDLE cmdPipeOut{ nullptr };
        HANDLE writeConsolePipeOut{ nullptr };
        HANDLE writeConsolePipeIn{ nullptr };
        HANDLE readConsolePipeOut{ nullptr };
        HANDLE readConsolePipeIn{ nullptr };

        SECURITY_ATTRIBUTES sa;
        ZeroMemory(&sa, sizeof(sa));
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = NULL;
        sa.nLength = sizeof(sa);

        assert(CreatePipe(&cmdPipeIn, &cmdPipeOut, &sa, 0));
        assert(CreatePipe(&writeConsolePipeIn, &writeConsolePipeOut, &sa, 0));
        assert(CreatePipe(&readConsolePipeIn, &readConsolePipeOut, &sa, 0));

        assert(SetHandleInformation(cmdPipeOut, HANDLE_FLAG_INHERIT, FALSE));
        assert(SetHandleInformation(writeConsolePipeOut, HANDLE_FLAG_INHERIT, FALSE));
        assert(SetHandleInformation(readConsolePipeIn, HANDLE_FLAG_INHERIT, FALSE));

        STARTUPINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi;


        int posSizeArgs[static_cast<int>(POS_SIZE_ARG::TOTAL)];
        posSizeArgs[static_cast<int>(POS_SIZE_ARG::POS_X)] = -500;
        posSizeArgs[static_cast<int>(POS_SIZE_ARG::POS_Y)] = 900;
        posSizeArgs[static_cast<int>(POS_SIZE_ARG::SIZE_X)] = 500;
        posSizeArgs[static_cast<int>(POS_SIZE_ARG::SIZE_Y)] = 500;

        std::string posSizeArgsStr[static_cast<int>(POS_SIZE_ARG::TOTAL)];

        for (int i{}; i < static_cast<int>(POS_SIZE_ARG::TOTAL); ++i)
        {
            if (posSizeArgs[i] < 0)
            {
                posSizeArgsStr[i].append("n");
            }
            posSizeArgsStr[i].append(std::to_string(std::abs(posSizeArgs[i])));
        }

        std::string cmdLine{ std::to_string(reinterpret_cast<intptr_t>(cmdPipeIn)) + " " +
            std::to_string(reinterpret_cast<intptr_t>(writeConsolePipeIn)) + " " +
            std::to_string(reinterpret_cast<intptr_t>(readConsolePipeOut)) + " " +
            posSizeArgsStr[static_cast<int>(POS_SIZE_ARG::SIZE_X)] + " " +
            posSizeArgsStr[static_cast<int>(POS_SIZE_ARG::SIZE_Y)] + " " +
            posSizeArgsStr[static_cast<int>(POS_SIZE_ARG::POS_X)] + " " +
            posSizeArgsStr[static_cast<int>(POS_SIZE_ARG::POS_Y)]
        };

        if (!CreateProcess("console.exe",
            cmdLine.data(),
            NULL,
            NULL,
            TRUE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si,
            &pi
        ))
        {
            throw std::runtime_error("error creating console process: " + std::to_string(GetLastError()));
        }
        WaitForInputIdle(pi.hProcess, INFINITE);

        CloseHandle(cmdPipeIn);
        CloseHandle(writeConsolePipeIn);
        CloseHandle(readConsolePipeOut);



        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::string message("hello world from new_parent!\n");
        WriteFile(writeConsolePipeOut, message.c_str(), message.size(), NULL, NULL);

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        char input[1024]{};

        WriteFile(cmdPipeOut, "r;1024;\\n;1;\n", 14, NULL, NULL);
        ReadFile(readConsolePipeIn, input, 1024, NULL, NULL);
        std::cout << "input read from winconsoles console: " << input << std::endl;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        WriteFile(cmdPipeOut, "e\n", 3, NULL, NULL);

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


    }
    catch (std::exception& e)
    {
        std::cerr << "exception thrown: " << e.what() << std::endl;
    }

}
