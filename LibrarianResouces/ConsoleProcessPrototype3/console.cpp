#include <iostream>
#include <fstream>
#include <string>
#include <concepts>
#include <thread>
#include <future>
#include <condition_variable>
#include <queue> 
#include <mutex>
#include <cstdio>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>


template<typename T>
concept stdI_or_O_stream = (std::same_as<T, std::istream> || std::same_as<T, std::ostream>);

template<typename T>
concept stdI_or_O_fstream = (std::same_as<T, std::ifstream> || std::same_as<T, std::ofstream>);


template<stdI_or_O_stream T, stdI_or_O_fstream U>
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

template<stdI_or_O_stream T, stdI_or_O_fstream U>
WinHANDLE_stdStreamAssociation<T, U>::WinHANDLE_stdStreamAssociation(HANDLE WinHndl)
{
    if (WinHndl == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Invalid Win32 HANDLE used to attempt to associate std::stream with Win32 HANDLE");
    m_fileDescriptor = _open_osfhandle(reinterpret_cast<intptr_t>(WinHndl), _O_TEXT);
    if (m_fileDescriptor == -1)
        throw std::runtime_error("could not create file descriptor from HANDLE");
    if constexpr (std::is_same<T, std::istream>::value)
    {
        m_fileStream = _fdopen(m_fileDescriptor, "r");
    }
    else
    {
        m_fileStream = _fdopen(m_fileDescriptor, "w");
    }
    if (m_fileStream == nullptr)
        throw std::runtime_error("coud not associate stream with file descriptor");
    m_fstream = std::make_unique<U>(m_fileStream);
    if (!m_fstream.get()->good())
        throw std::runtime_error("could not create valid std::i/ofstream from FILE*");
    m_stream = std::make_unique<T>(m_fstream.get()->rdbuf());
    if (!m_stream.get()->good())
        throw std::runtime_error("could not create valid std::i/ostream from std::i/ofstream");
}

template<stdI_or_O_stream T, stdI_or_O_fstream U>
WinHANDLE_stdStreamAssociation<T, U>::~WinHANDLE_stdStreamAssociation()
{
    fclose(m_fileStream);
    if (m_fstream.get()->is_open())
    {
        m_fstream.get()->close();
    }
}


void writeToConsole(WinHANDLE_stdStreamAssociation<std::istream, std::ifstream>& inStream, bool& bExit);

struct ReadThreadCmdQueue {
    std::queue<char> queue;
    std::mutex mutex;
    std::condition_variable cv;
    bool bCmdRead{ false };
}

void readFromConsole(WinHANDLE_stdStreamAssociation<std::ostream, std::ofstream>& outStream, ReadThreadCmdQueue& cmdQueue);

int main(int argc, char* argv[])
{
    //temp line to stop execution on entry to wait for debugger attach
    while (!IsDebuggerPresent())
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::cout << "Debugger Present" << std::endl;

    if (argc != 3)
        throw std::runtime_error("invalid number of arguments passed to console process");

    WinHANDLE_stdStreamAssociation<std::istream, std::ifstream> parentCmdIn(reinterpret_cast<HANDLE>(std::stoll(argv[0])));

    WinHANDLE_stdStreamAssociation<std::istream, std::ifstream> consoleWriteIn(reinterpret_cast<HANDLE>(std::stoll(argv[1])));

    WinHANDLE_stdStreamAssociation<std::ostream, std::ofstream> consoleReadOut(reinterpret_cast<HANDLE>(std::stoll(argv[2])));

    //start read thread 
    //start write thread

    bool bWriteThreadExit{ false };

    ReadThreadCmdQueue readThreadCmdQueue;

    auto writeThreadFuture = std::async(
        std::launch::async,
        writeToConsole,
        std::ref(consoleWriteIn),
        std::ref(bWriteThreadExit)
    );



    bool bExit{ false };
    char c{};
    do {
        c = parentCmdIn.get().get();
        if (c == 'e')
        {
            bWriteThreadExit = true;
            writeThreadFuture.wait();
            bExit = true;
        }
    } while (!bExit);


    std::cout << "press enter to exit console..." << std::endl;
    std::cin.ignore(1000, '\n');
    return 0;
}

void writeToConsole(WinHANDLE_stdStreamAssociation<std::istream, std::ifstream>& inStream, bool& bExit)
{
    do
    {
        std::cout.put(inStream.get().get());
    } while (!bExit);
}

void readFromConsole(WinHANDLE_stdStreamAssociation<std::ostream, std::ofstream>& outStream, ReadThreadCmdQueue& cmdQueue)
{
    bool bExit{ false };
    char cmd{};
    do {
        {
            std::unique_lock<std::mutex> lock(cmdQueue.mutex);
            if (!cmdQueue.queue.size())
                cmdQueue.cv.wait();
            cmd = cmdQueue.queue.front();
            cmdQueue.queue.pop();
            cmdQueue.bCmdRead = true;
        }
        if (cmd == 'e')
        {
            bExit = true;
        }
        else if (cmd == 'g')
        {
            outStream.get().put(std::cin.get());
        }
        else if (cmd == 'i')
        {
            std::cin.ignore()
        }
    } while (!bExit);
}
