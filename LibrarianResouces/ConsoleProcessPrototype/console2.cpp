#include <iostream>
#include <string>
#include <fstream>
#include <concepts>
#include <memory>
#include <cassert>
#include <thread>
#include <mutex>
#include <type_traits>
#include <queue>
#include <io.h>
#include <Windows.h>

template<typename T, typename U>
concept StdIO_Stream = (std::same_as<T, std::istream> && std::same_as<U, std::ifstream>)
|| (std::same_as<T, std::ostream> && std::same_as<U, std::ofstream>);


template<typename T, typename U>
    requires StdIO_Stream<T, U>
class WinHANDLE_stdStreamAssociation
{
public:
    WinHANDLE_stdStreamAssociation(HANDLE WinHndl);
    ~WinHANDLE_stdStreamAssociation();
    T& get() { return *m_stream.get(); }
private:
    WinHANDLE_stdStreamAssociation(const WinHANDLE_stdStreamAssociation&) = delete;
    WinHANDLE_stdStreamAssociation(WinHANDLE_stdStreamAssociation&&) = delete;
    int m_fileDescriptor{ -1 };
    FILE* m_fileStream{ nullptr };
    std::unique_ptr<U> m_fstream{ nullptr };
    std::unique_ptr<T> m_stream{ nullptr };
};

template<typename T, typename U>
    requires StdIO_Stream<T, U>
WinHANDLE_stdStreamAssociation<T, U>::WinHANDLE_stdStreamAssociation(HANDLE WinHndl)
{
    if (WinHndl == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Invalid WHANDLE");
    m_fileDescriptor = _open_osfhandle(reinterpret_cast<intptr_t>(WinHndl), 0);
    if (m_fileDescriptor == -1)
        throw std::runtime_error("could not create file descriptor from HANDLE");
    if constexpr (std::is_same<T, std::istream>::value)
    {
        m_fileStream = _fdopen(m_fileDescriptor, "r");
    }
    else
    {
        m_fileStream = _fdopen(m_fileDescriptor, "a");
    }
    if (m_fileStream == nullptr)
        throw std::runtime_error("coud not associate stream with file descriptor");
    m_fstream = std::make_unique<U>(m_fileStream);
    if (!m_fstream.get()->good())
        throw std::runtime_error("could not create valid std::i/ofstream");
    m_stream = std::make_unique<T>(m_fstream.get()->rdbuf());
    if (!m_stream.get()->good())
        throw std::runtime_error("could not create valid std::i/ostream");
}

template<typename T, typename U>
    requires StdIO_Stream<T, U>
WinHANDLE_stdStreamAssociation<T, U>::~WinHANDLE_stdStreamAssociation()
{
    assert(fclose(m_fileStream) != EOF);
    if (m_fstream.get()->is_open())
    {
        std::cout << "m_fstream needed closing, del this print\n";
        m_fstream.get()->close();
    }
}


class Console {
public:
    static bool Init();
    static void StartIO();
private:
    inline static std::istream sm_parentInStream{ std::cin.rdbuf() };
    inline static std::ostream sm_parentOutStream{ std::cout.rdbuf() };
    inline static std::istream sm_cmdInStream{};

    inline static std::queue<char> sm_readThreadQueue{};
    inline static std::queue<char> sm_writeThreadQueue{};

    inline static WinHANDLE_stdStreamAssociation<std::ostream, std::ofstream> cout{ CreateFileA(
        "CONOUT$",
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL) };
    inline static WinHANDLE_stdStreamAssociation<std::istream, std::ifstream> cin{ CreateFileA(
        "CONIN$",
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL) };

    static void pollForReadCmd();
    static void pollForWriteCmd();
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

    std::cout.rdbuf(cout.get().rdbuf());
    std::cin.rdbuf(cin.get().rdbuf());

    return true;
}

void Console::StartIO()
{

    bool bExit{ false };
    do
    {
        /*
            peek cmd stream
            if peek is eof then send close command to threads and exit loop
            else if peek is e then send close command to threads and exit loop
            else if peek is r then tell the read thread to read from CONIN to inherited out stream
            else if peek is w then tell the write thread to write to CONOUT from inherited in stream
        */

        // ### peek cmd stream
        char c = static_cast<char>(sm_cmdInStream.peek());
        if (c == std::istream::traits_type::eof() || c == 'e')
        {
            bExit = true;
            //add exit signifier to write thread queue
            //add exit signifier to read thread queue

            //wait for threads to join
        }
        else if (c == 'r')
        {
            //push read signifier to read thread queue
        }
        else if (c == 'w')
        {
            //push write signifier to write thread queue
        }
    } while (!bExit);
    return;
}

void Console::pollForReadCmd() {
    bool bExit{ false };
    do {



    } while (!bExit)
}

void Console::pollForWriteCmd() {

}
