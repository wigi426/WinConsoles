#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <concepts>
#include <memory>
#include <cassert>
#include <type_traits>
#include <queue>
#include <mutex>
#include <future>
#include <condition_variable>
#include <io.h>
#include <Windows.h>


template<std::signed_integral T>
inline T stosi(const std::string& str)
{
    if constexpr (sizeof(T) == sizeof(int))
        return std::stoi(str);
    else if constexpr (sizeof(T) == sizeof(long))
        return std::stol(str);
    else if constexpr (sizeof(T) == sizeof(long long))
        return std::stoll(str);
    else
        return std::stoi(str);
}

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
    T::int_type eofVal() { return T::traits_type::eof(); }
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
    static bool Init(const std::string cmdHandleArg);
    static void StartIO();
private:
    inline static constexpr std::streamsize BUFF_SIZE{ 5000 };

    inline static std::istream sm_parentInStream{ std::cin.rdbuf() };
    inline static std::ostream sm_parentOutStream{ std::cout.rdbuf() };

    inline static WinHANDLE_stdStreamAssociation<std::ostream, std::ofstream> sm_cout{ CreateFileA(
        "CONOUT$",
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL) };
    inline static WinHANDLE_stdStreamAssociation<std::istream, std::ifstream> sm_cin{ CreateFileA(
        "CONIN$",
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL) };
    inline static std::unique_ptr<WinHANDLE_stdStreamAssociation<std::istream, std::ifstream>> sm_parentCmd{ nullptr };

    inline static std::queue<char> sm_readCmdQueue{};
    inline static std::mutex sm_readCmdQueue_mutex{};
    inline static std::condition_variable sm_readCmdQueue_cv{};
    inline static bool sm_readCmdRead{ false };

    inline static std::queue<char> sm_writeCmdQueue{};
    inline static std::mutex sm_writeCmdQueue_mutex{};
    inline static std::condition_variable sm_writeCmdQueue_cv{};
    inline static bool sm_writeCmdRead{ false };

    static void readFromParent();
    static void writeToParent();
};

int main(int argc, char* argv[]) {
    if (argc != 1)
        throw std::runtime_error("incorrect number of command line arguments");
    if (!Console::Init(argv[0]))
        throw std::runtime_error("failed to initalize console");
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    Console::StartIO();
    return 0;
}



bool Console::Init(const std::string cmdHandleArg)
{
    if (!sm_parentInStream.good())
        return false;
    if (!sm_parentOutStream.good())
        return false;

    //redirect cout and cin back to the console
    std::cout.rdbuf(sm_cout.get().rdbuf());
    std::cin.rdbuf(sm_cin.get().rdbuf());

    sm_parentCmd = std::make_unique<WinHANDLE_stdStreamAssociation<std::istream, std::ifstream>>(reinterpret_cast<HANDLE>(stosi<intptr_t>(cmdHandleArg)));

    return true;
}

void Console::StartIO()
{
    auto readThreadFuture = std::async(std::launch::async, readFromParent);
    auto writeThreadFuture = std::async(std::launch::async, writeToParent);


    bool bExit{ false };
    auto eof{ sm_parentCmd.get()->eofVal() };
    auto cmd{ sm_parentCmd.get()->get().peek() };
    do
    {
        cmd = sm_parentCmd.get()->get().get();
        if (cmd == 'e' || cmd == eof || !sm_parentCmd)
        {
            {
                std::lock_guard<std::mutex> lock(sm_readCmdQueue_mutex);
                sm_readCmdQueue.push('e');
            }
            sm_readCmdQueue_cv.notify_one();
            {
                std::unique_lock<std::mutex> lock(sm_readCmdQueue_mutex);
                sm_readCmdQueue_cv.wait(lock, [] {return sm_readCmdRead;});
                sm_readCmdRead = false;
            }
            if (readThreadFuture.wait_for(std::chrono::seconds(1)) != std::future_status::ready)
                throw std::runtime_error("read thread did not exit correctly");

            {
                std::lock_guard<std::mutex> lock(sm_writeCmdQueue_mutex);
                sm_writeCmdQueue.push('e');
            }
            sm_writeCmdQueue_cv.notify_one();
            {
                std::unique_lock<std::mutex> lock(sm_writeCmdQueue_mutex);
                sm_writeCmdQueue_cv.wait(lock, [] {return sm_writeCmdRead;});
                sm_writeCmdRead = false;
            }
            if (writeThreadFuture.wait_for(std::chrono::seconds(1)) != std::future_status::ready)
                throw std::runtime_error("write thread did not exit correctly");

            bExit = true;
        }
        else if (cmd == 'r')
        {
            {
                std::lock_guard<std::mutex> lock(sm_readCmdQueue_mutex);
                sm_readCmdQueue.push('r');
            }
            sm_readCmdQueue_cv.notify_one();
            {
                std::unique_lock<std::mutex> lock(sm_readCmdQueue_mutex);
                sm_readCmdQueue_cv.wait(lock, [] {return sm_readCmdRead;});
                sm_readCmdRead = false;
            }
        }
        else if (cmd == 'w')
        {
            {
                std::lock_guard<std::mutex> lock(sm_writeCmdQueue_mutex);
                sm_writeCmdQueue.push('r');
            }
            sm_writeCmdQueue_cv.notify_one();
            {
                std::unique_lock<std::mutex> lock(sm_writeCmdQueue_mutex);
                sm_writeCmdQueue_cv.wait(lock, [] {return sm_writeCmdRead;});
                sm_writeCmdRead = false;
            }
        }
    } while (!bExit);
    return;
}

void Console::readFromParent()
{
    try {
        bool bExit{ false };
        std::string buff;
        buff.resize(BUFF_SIZE);
        do {
            std::unique_lock<std::mutex> lock(sm_readCmdQueue_mutex);
            if (!sm_readCmdQueue.size())
                sm_readCmdQueue_cv.wait(lock);
            char cmd = sm_readCmdQueue.front();
            sm_readCmdQueue.pop();
            sm_readCmdRead = true;
            lock.unlock();
            sm_readCmdQueue_cv.notify_one();
            if (cmd == 'e')
            {
                bExit = true;
            }
            else if (cmd == 'r')
            {
                std::cout << "heya";
                sm_parentInStream >> buff;
                std::cout << buff << std::endl;
            }
        } while (!bExit);
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

void Console::writeToParent()
{
    bool bExit{ false };
    std::string buff;
    buff.resize(BUFF_SIZE);
    do {
        std::unique_lock<std::mutex> lock(sm_writeCmdQueue_mutex);
        if (!sm_writeCmdQueue.size())
            sm_writeCmdQueue_cv.wait(lock);
        char cmd = sm_writeCmdQueue.front();
        sm_writeCmdQueue.pop();
        sm_writeCmdRead = true;
        lock.unlock();
        sm_writeCmdQueue_cv.notify_one();
        if (cmd == 'e')
        {
            bExit = true;
        }
        else if (cmd == 'w')
        {
            std::cin.read(buff.data(), BUFF_SIZE);
            sm_parentOutStream.write(buff.data(), BUFF_SIZE);
        }
    } while (!bExit);
}