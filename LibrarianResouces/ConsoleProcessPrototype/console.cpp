#include <iostream>
#include <string>
#include <fstream>
#include <io.h>
#include <Windows.h>



struct fileHANDLE {
    fileHANDLE(_In_ LPCSTR lpFileName,
        _In_ DWORD dwDesiredAccess,
        _In_ DWORD dwShareMode,
        _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        _In_ DWORD dwCreationDisposition,
        _In_ DWORD dwFlagsAndAttributes,
        _In_opt_ HANDLE hTemplateFile) {
        h = CreateFileA(lpFileName,
            dwDesiredAccess,
            dwShareMode,
            lpSecurityAttributes,
            dwCreationDisposition,
            dwFlagsAndAttributes,
            hTemplateFile);
        if (h == INVALID_HANDLE_VALUE)
            throw std::runtime_error(std::string("CreateFileA failed on file: \"") + lpFileName + '\"');
    }
    ~fileHANDLE()
    {
        try {
            CloseHandle(h);
        }
        catch (std::exception& e)
        {
            std::cout << "caught exception in fileHANLDE when closing assosiated win32 HANDLE, GetLastError(): " << GetLastError() << '\n' << "exception::what(): " << e.what() << '\n';
        }
    }
    HANDLE get() { return h; }

private:
    HANDLE h{ nullptr };
};

class Console {
public:
    static bool Init();
    static void StartIO();
private:
    inline static std::istream sm_parentInStream{ std::cin.rdbuf() };
    inline static std::ostream sm_parentOutStream{ std::cout.rdbuf() };

    inline static fileHANDLE sm_coutFileHandle{ "CONOUT$", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL };
    inline static fileHANDLE sm_cinFileHandle{ "CONIN$", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL };

    inline static int coutFD{ _open_osfhandle(reinterpret_cast<intptr_t>(sm_coutFileHandle.get()), 0) };
    inline static int cinFD{ _open_osfhandle(reinterpret_cast<intptr_t>(sm_cinFileHandle.get()), 0) };

    inline static FILE* coutFile{ _fdopen(coutFD, "a") };
    inline static FILE* cinFile{ _fdopen(cinFD, "r") };

    inline static std::ofstream fcout{ coutFile };
    inline static std::ifstream fcin{ cinFile };

};

int main() {
    if (!Console::Init())
        throw std::runtime_error("failed to initalize console");
    Console::StartIO();
    return 0;
}

bool Console::Init()
{
    if (!sm_parentInStream.good())
        return false;
    if (!sm_parentOutStream.good())
        return false;
    std::cin.rdbuf(fcin.rdbuf());
    if (!std::cin.good())
        return false;
    std::cout.rdbuf(fcout.rdbuf());
    if (!std::cout.good())
        return false;

    return true;
}

void Console::StartIO()
{
    bool bExit{ false };
    std::string inBuff{};
    std::string inputBuff{};
    std::string exitCmd{ "exit" };
    std::string writeCmd{ "write" };
    std::string readCmd{ "read" };
    do
    {
        std::getline(sm_parentInStream, inBuff);
        if (inBuff.starts_with(exitCmd))
            bExit = true;
        else if (inBuff.starts_with(writeCmd))
            std::cout << inBuff.substr(writeCmd.size(), inBuff.size() - writeCmd.size()) << '\n';
        else if (inBuff.starts_with(readCmd))
        {
            std::getline(std::cin, inputBuff);
            inputBuff.append("\n");
            sm_parentOutStream.write(inputBuff.c_str(), inputBuff.size());
            sm_parentOutStream.flush();
        }

    } while (!bExit);
    return;
}