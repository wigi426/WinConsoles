#include <iostream>
#include <streambuf>
#include <string>
#include <algorithm>
#include <cassert>
#include <Windows.h>
#include <processthreadsapi.h>
#include <namedpipeapi.h>
#include <Win32Toolbelt.h>

class usercin: private std::istream
{
private:
    WinHANDLE_stdStreamAssociation<std::ostream, std::ofstream> cmdOut;
public:
    usercin(std::streambuf* buf, HANDLE cmdOutHndl): std::istream::basic_istream(buf), cmdOut{ cmdOutHndl } {}

    void exitConsole() {
        cmdOut.get().write("e\n", 3);
        cmdOut.get().flush();
    }

    basic_istream& ignore(std::streamsize count = 1, int_type delim = traits_type::eof())
    {
        std::string cmd{ "r;" + std::to_string(count) + ";" };
        if (delim == '\n')
            cmd.append("\\n");
        else
            cmd.append(std::to_string(delim));
        cmd.append(";1;\n");
        cmdOut.get().write(cmd.c_str(), cmd.size());
        cmdOut.get().flush();
        std::istream::ignore(count, delim);
        return std::istream::ignore(count, delim);
    }


    int_type get()
    {
        cmdOut.get().write("r;2;\\n;0;\n", 10);
        cmdOut.get().flush();
        return std::istream::get();
    }
    basic_istream& get(char_type& ch)
    {
        cmdOut.get().write("r;2;\\n;0;\n", 10);
        cmdOut.get().flush();
        return std::istream::get(ch);
    }
    basic_istream& get(char_type* s, std::streamsize count)
    {
        std::string cmd = "r;" + std::to_string(count) + ";\\n;0;\n";
        cmdOut.get().write(cmd.c_str(), cmd.size());
        cmdOut.get().flush();
        return std::istream::get(s, count);
    }
    basic_istream& get(char_type* s, std::streamsize count, char_type delim)
    {
        std::string cmd = "r;" + std::to_string(count);
        if (delim == '\0')
            cmd.append(";\\0");
        else if (delim == '\n')
            cmd.append(";\\n");
        else
            cmd.append(";" + delim);
        cmd.append(";0;\n");
        cmdOut.get().write(cmd.c_str(), cmd.size());
        cmdOut.get().flush();
        return std::istream::get(s, count, delim);
    }
    basic_istream& get(std::streambuf& strbuf)
    {
        std::string cmd = "r;" + std::to_string(std::numeric_limits<std::streamsize>::max()) + ";\\n;0;\n";
        cmdOut.get().write(cmd.c_str(), cmd.size());
        cmdOut.get().flush();
        return std::istream::get(strbuf);
    }
    basic_istream& get(std::streambuf& strbuf, char_type delim)
    {
        std::string cmd = "r;" + std::to_string(std::numeric_limits<std::streamsize>::max());
        if (delim == '\n')
            cmd.append(";\\n");
        else
            cmd.append(";" + delim);
        cmd.append(";0;\n");
        cmdOut.get().write(cmd.c_str(), cmd.size());
        cmdOut.get().flush();
        return std::istream::get(strbuf, delim);
    }

    template<typename T>
    basic_istream& operator>>(T& value) {
        cmdOut.get().write("r;65532;\\n;0;\n", 13);
        cmdOut.get().flush();
        return std::istream::operator>>(value);
    }

    basic_istream& operator>>(short& value) { return operator>><std::remove_reference<decltype(value)>::type>(value); }
    basic_istream& operator>>(unsigned short& value) { return  operator>><std::remove_reference<decltype(value)>::type>(value); }

    basic_istream& operator>>(int& value) { return  operator>><std::remove_reference<decltype(value)>::type>(value); }
    basic_istream& operator>>(unsigned int& value) { return operator>><std::remove_reference<decltype(value)>::type>(value); }

    basic_istream& operator>>(long& value) { return  operator>><std::remove_reference<decltype(value)>::type>(value); }
    basic_istream& operator>>(unsigned long& value) { return operator>><std::remove_reference<decltype(value)>::type>(value); }

    basic_istream& operator>>(long long& value) { return operator>><std::remove_reference<decltype(value)>::type>(value); }
    basic_istream& operator>>(unsigned long long& value) { return  operator>><std::remove_reference<decltype(value)>::type>(value); }

    basic_istream& operator>>(float& value) { return  operator>><std::remove_reference<decltype(value)>::type>(value); }
    basic_istream& operator>>(double& value) { return operator>><std::remove_reference<decltype(value)>::type>(value); }
    basic_istream& operator>>(long double& value) { return  operator>><std::remove_reference<decltype(value)>::type>(value); }

    basic_istream& operator>>(bool& value) { return  operator>><std::remove_reference<decltype(value)>::type>(value); }
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

        std::string cmdLine{ std::to_string(reinterpret_cast<intptr_t>(cmdPipeIn)) + " " +
                std::to_string(reinterpret_cast<intptr_t>(writeConsolePipeIn)) + " " +
                std::to_string(reinterpret_cast<intptr_t>(readConsolePipeOut)) };

        assert(CreateProcess("console.exe",
            cmdLine.data(),
            NULL,
            NULL,
            TRUE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si,
            &pi
        ));

        WaitForInputIdle(pi.hProcess, INFINITE);

        CloseHandle(cmdPipeIn);
        CloseHandle(writeConsolePipeIn);
        CloseHandle(readConsolePipeOut);

        {
            WinHANDLE_stdStreamAssociation<std::ostream, std::ofstream> writeConsoleOut(writeConsolePipeOut);
            WinHANDLE_stdStreamAssociation<std::istream, std::ifstream> readConsoleIn(readConsolePipeIn);
            usercin cin(readConsoleIn.get().rdbuf(), cmdPipeOut);



            char buff[500]{};
            cin.get(buff, 500);


            std::cout << buff << std::endl;

            std::cin.ignore(1000, '\n');

            cin.exitConsole();
        }
        std::cin.ignore(1000, '\n');

    }
    catch (std::exception& e)
    {
        std::cout << "caughed exception: " << e.what() << std::endl;
    }
    return 0;
}