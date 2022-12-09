#include <iostream>
#include <streambuf>
#include <string>
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

    int_type get()
    {
        cmdOut.get().write("ru;2;\\n;\n", 10);
        cmdOut.get().flush();
        return std::istream::get();
    }
    basic_istream& get(char_type& ch);
    basic_istream& get(char_type* s, std::streamsize count);
    basic_istream& get(char_type* s, std::streamsize count, char_type delim);
    basic_istream& get(std::streambuf& strbuf);
    basic_istream& get(std::streambuf& strbuf, char_type delim);

    template<typename T>
    basic_istream& operator>>(T& value) {
        cmdOut.get().write("rf", 3);
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

    WinHANDLE_stdStreamAssociation<std::ostream, std::ofstream> writeConsoleOut(writeConsolePipeOut);
    WinHANDLE_stdStreamAssociation<std::istream, std::ifstream> readConsoleIn(readConsolePipeIn);


    //simulating get in an ecapsulated class
    usercin cin(readConsoleIn.get().rdbuf(), cmdPipeOut);
    int i;
    cin >> i;
    std::cout << i << std::endl;




    /*
        //simulating user call stream.get(buff, 10, '\n');
        std::string cmd{ "rg;10;\\n;\n" };
        cmdOut.get().write(cmd.c_str(), cmd.size());
        cmdOut.get().flush();
        char buff[10];
        readConsoleIn.get().get(buff, std::streamsize(10), char('/0'));
        std::cout << "read from console:" << buff << std::endl;
        readConsoleIn.get().ignore(std::numeric_limits<std::streamsize>::max(), '\0');

        //simulating user call stream.get();
        cmd = "rg;2;\\n;\n";
        cmdOut.get().write(cmd.c_str(), cmd.size());
        cmdOut.get().flush();
        int read = readConsoleIn.get().get();
        std::cout << "read int:" << read << std::endl;
    */

    //simulating user call stream.get(*std::cout.rdbuf(), '\n');

    /*
    decltype(*std::cout.rdbuf())& streamBuf = *std::cout.rdbuf();
    std::streamsize count = std::numeric_limits<decltype(count)>::max();
    std::string cmd{ "rg;" };
    cmd.append(std::to_string(count) + ";\\n;\n");
    cmdOut.get().write(cmd.c_str(), cmd.size());
    cmdOut.get().flush();
    readConsoleIn.get().get(streamBuf, '\n');
*/

    std::cin.ignore(1000, '\n');




    return 0;
}