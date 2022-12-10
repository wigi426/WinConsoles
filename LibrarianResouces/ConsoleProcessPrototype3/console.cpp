#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
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
    std::queue<std::string> queue;
    std::mutex mutex;
    std::condition_variable cv;
    bool bCmdRead{ false };
};

void readFromConsole(WinHANDLE_stdStreamAssociation<std::ostream, std::ofstream>& outStream, ReadThreadCmdQueue& cmdQueue);

int main(int argc, char* argv[])
{
    //temp line to stop execution on entry to wait for debugger attach
    // while (!IsDebuggerPresent())
    //     std::this_thread::sleep_for(std::chrono::milliseconds(200));

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

    auto readThreadFuture = std::async(
        std::launch::async,
        readFromConsole,
        std::ref(consoleReadOut),
        std::ref(readThreadCmdQueue)
    );



    bool bExit{ false };
    constexpr uint8_t buffSize = std::numeric_limits<decltype(buffSize)>::max();
    std::string cmdBuff{};
    cmdBuff.resize(buffSize);
    do {
        //if read thread exited itself
        if (readThreadFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
        {
            bWriteThreadExit = true;
            writeThreadFuture.wait();
            bExit = true;
        }
        parentCmdIn.get().getline(cmdBuff.data(), cmdBuff.size());
        if (parentCmdIn.get().rdstate() != parentCmdIn.get().goodbit)
        {
            bWriteThreadExit = true;
            if (readThreadFuture.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
            {
                {
                    std::lock_guard lock(readThreadCmdQueue.mutex);
                    readThreadCmdQueue.queue.push("e");
                }
                readThreadCmdQueue.cv.notify_one();
                {
                    std::unique_lock lock(readThreadCmdQueue.mutex);
                    readThreadCmdQueue.cv.wait(lock, [&] {return readThreadCmdQueue.bCmdRead; });
                    readThreadCmdQueue.bCmdRead = false;
                }
                writeThreadFuture.wait();
            }
            readThreadFuture.wait();
            bExit = true;
        }
        else if (!parentCmdIn.get().gcount())
        {
            throw std::runtime_error("empty command sent to console");
        }
        else if (cmdBuff.at(0) == 'e')
        {
            bWriteThreadExit = true;
            {
                std::lock_guard lock(readThreadCmdQueue.mutex);
                readThreadCmdQueue.queue.push("e");
            }
            readThreadCmdQueue.cv.notify_one();
            {
                std::unique_lock lock(readThreadCmdQueue.mutex);
                readThreadCmdQueue.cv.wait(lock, [&] {return readThreadCmdQueue.bCmdRead; });
                readThreadCmdQueue.bCmdRead = false;
            }
            writeThreadFuture.wait();
            readThreadFuture.wait();
            bExit = true;
        }
        else if (cmdBuff.at(0) == 'r')
        {

            {
                std::lock_guard lock(readThreadCmdQueue.mutex);
                readThreadCmdQueue.queue.push(cmdBuff.substr(1, parentCmdIn.get().gcount() - 1));
            }
            readThreadCmdQueue.cv.notify_one();
            {
                std::unique_lock lock(readThreadCmdQueue.mutex);
                readThreadCmdQueue.cv.wait(lock, [&] {return readThreadCmdQueue.bCmdRead; });
                readThreadCmdQueue.bCmdRead = false;
            }
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
        if (inStream.get().rdstate() != inStream.get().goodbit)
            bExit = true;
        else
            std::cout.put(inStream.get().get());
    } while (!bExit);
}

enum getCommandArgsIndex {
    GETCMDARG_COUNT = 0,
    GETCMDARG_DELIM = 1,
    GETCMDARG_TOTAL
};

enum uCommandArgsIndex {
    UCMDARG_COUNT = 0,
    UCMDARG_DELIM = 1,
    UCMDARG_TOTAL
};

void readFromConsole(WinHANDLE_stdStreamAssociation<std::ostream, std::ofstream>& outStream, ReadThreadCmdQueue& cmdQueue)
{
    bool bExit{ false };
    std::string cmd{};
    std::string proxyString;
    proxyString.resize(std::numeric_limits<int16_t>::max());
    do {
        {
            std::unique_lock<std::mutex> lock(cmdQueue.mutex);
            if (!cmdQueue.queue.size())
                cmdQueue.cv.wait(lock);
            cmd = std::move(cmdQueue.queue.front());
            cmdQueue.queue.pop();
            cmdQueue.bCmdRead = true;
        }
        cmdQueue.cv.notify_one();
        if (!cmd.size())
        {
            throw std::runtime_error("empty command send to read thread");
        }
        if (cmd.at(0) == 'e')
        {
            bExit = true;
        }
        else if (cmd.at(0) == 'u')
        {
            //testing u, as generic read command for unformatted input functions, since most istream input functions can fulfil their output with 2 pieces of info: count and delim

            //list of all std::basic_istream unformatted input functions, and what information is needed on the console end for each on to be replicated:

            /*generic data:
                int count
                int delim
            */

            /*  get() overloads:
            int_type get();                                                                     data Required: count(1+delim(1))
            basic_istream& get(char_type & ch);                                                 data Required: count(1+delim(1))
            basic_istream& get(char_type * s, std::streamsize count);                           data Required: count delim('\n')
            basic_istream& get(char_type * s, std::streamsize count, char_type delim);          data Required: count delim
            basic_istream& get(basic_streambuf & strbuf, char_type delim);                      data Required: count(streamsize) delim
            basic_istream& get(basic_streambuf & strbuf);                                       data Required: count(streamsize) delim('\n')
            */
            /*  peek() overloads:
            int_type peek();                                                                    data Required: count(1+dleim(1))
            /* unget() overloads:
            basic_istream& unget();                                                             data Required: none, console isn't concerned with this function being called on the parent side.
                                                                                                                    console isn't responsible for replicating this behaviour
            */
            /* putback() overlaods:
            basic_istream& putback( char_type ch );                                              data Required: none, console isn't concerned with this function being called on the parent side.
                                                                                                                    console isn't responsible for replicating this behaviour
            */
            /* getline() overloads:
            basic_istream& getline( char_type* s, std::streamsize count );                       data Required: count delim('\n')
            basic_istream& getline( char_type* s, std::streamsize count, char_type delim );      data Required: count delim
            */
            /* ignore() overloads:
            basic_istream& ignore( std::streamsize count = 1, int_type delim = Traits::eof() );  data Required: count delim
            */
            /* read() overloads:
            basic_istream& read( char_type* s, std::streamsize count );                          data Required: count
            */
            /* readsome() overloads:
            std::streamsize readsome( char_type* s, std::streamsize count );                     data Required: none, parent responsible for implementing replicant behaviour
            */


            /*
             unformatted input command format:
             u;count;delim;
             might look like:
             u;10;\\n;
            */


            std::string cmdArgs[UCMDARG_TOTAL]{};
            size_t seperatorPos{};
            size_t nextSeperatorPos{};
            for (int i{}; i < GETCMDARG_TOTAL; ++i)
            {
                seperatorPos = cmd.find_first_of(';');
                nextSeperatorPos = cmd.find_first_of(';', seperatorPos + 1);
                if (seperatorPos == cmd.npos || nextSeperatorPos == cmd.npos)
                    throw std::runtime_error("read command 'g' was not followed by enough arguments, or formatting of ';' was incorrect");
                cmdArgs[i] = cmd.substr(seperatorPos + 1, nextSeperatorPos - (seperatorPos + 1));
                cmd.erase(seperatorPos, nextSeperatorPos - seperatorPos);
            }
            std::streamsize count = std::stoll(cmdArgs[UCMDARG_COUNT]);
            char delim{};
            if (cmdArgs[UCMDARG_DELIM].compare("\\n") == 0)
                delim = '\n';
            else
                delim = cmdArgs[UCMDARG_DELIM].at(0);

            //make sure count is not larger than the size of the proxy string
            //otherwise the get() function will attempt to write input to an out of range address
            if (count > static_cast<std::streamsize>(proxyString.size()))
                count = static_cast<std::streamsize>(proxyString.size());

            std::cin.get(proxyString.data(), count, delim);


            if (static_cast<std::streamsize>(proxyString.size()) > std::cin.gcount())
            {
                proxyString.at(std::cin.gcount()) = delim;
                count = std::cin.gcount() + 1;
                if (static_cast<std::streamsize>(proxyString.size()) > std::cin.gcount() + 1)
                {
                    proxyString.at(std::cin.gcount() + 1) = '\0';
                }
            }
            else
            {
                proxyString.back() = delim;
                count = proxyString.size();
            }


            if (std::cin.rdstate() != std::cin.goodbit)
            {
                if (std::cin.bad() | std::cin.eof())
                {
                    bExit = true;
                    outStream.~WinHANDLE_stdStreamAssociation();
                }
                else {
                    std::cin.clear();
                    outStream.get().write(proxyString.c_str(), count);
                    outStream.get().flush();
                }
            }
            else
            {
                outStream.get().write(proxyString.c_str(), count);
                outStream.get().flush();
            }
        }
        else if (cmd.at(0) == 'i')
        {
            std::cin.ignore();
        }
    } while (!bExit);

}
