#include <stdexcept>
#include <string>
#include <queue>
#include <mutex>
#include <future>
#include <condition_variable>
#include <utility>
#include <iostream>
#include <algorithm>
#include "../Win32Helpers.h"

class reportToUser_exception: public std::exception {
public:
    reportToUser_exception(const char* const message): std::exception(message) {}
    reportToUser_exception(const reportToUser_exception& other): std::exception(other) {}
    reportToUser_exception& operator=(const reportToUser_exception& other) {
        std::exception::operator=(other);
        return *this;
    }
};
class Pipe {
public:
    class pipe_exception: public std::exception {
    public:
        pipe_exception(const char* const message): std::exception(message) {}
        pipe_exception(const pipe_exception& other): std::exception(other) {}
        pipe_exception& operator=(const pipe_exception& other) {
            std::exception::operator=(other);
            return *this;
        }
    };
protected:
    Pipe(Win32Helpers::Hndl&& pipeHndl): m_pipeHndl(std::move(pipeHndl)) {}
    Win32Helpers::Hndl m_pipeHndl;
};

class InputPipe: public Pipe {
private:
    std::string m_buffer;
    DWORD m_endChar;
public:
    InputPipe(Win32Helpers::Hndl&& pipeHndl): Pipe(std::move(pipeHndl)), m_endChar{ 0 } {
        m_buffer.resize(1024);
    }
    void read(std::string& buffer, std::streamsize count, char delim) {
        if (!count)
            return;
        if (m_endChar == 0)//if the buffer is empty extract from pipe
        {

            static DWORD bytesRead{ 0 };
            if (!ReadFile(m_pipeHndl.get(),
                m_buffer.data(),
                static_cast<DWORD>(std::clamp<std::streamsize>(count, 0, std::numeric_limits<DWORD>::max())),
                &bytesRead,
                NULL))
                throw pipe_exception("ReadFile() encountered an error when reading from pipe");
            m_endChar = bytesRead;
        }
        size_t delimPos{ m_buffer.find_first_of(delim, m_endChar) };
        if (std::string::npos == delimPos) //if delim is not in the read characters
        {
            buffer = m_buffer.substr(0, m_endChar);
            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + m_endChar);
            m_endChar = 0;
        }
        else
        {
            buffer = m_buffer.substr(0, delimPos);
            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + delimPos);
            m_endChar -= static_cast<DWORD>(std::clamp<size_t>(delimPos, 0, std::numeric_limits<DWORD>::max()));
        }
    }
};

class OutputPipe: public Pipe {
public:
    OutputPipe(Win32Helpers::Hndl&& pipeHndl): Pipe(std::move(pipeHndl)) {}
    void write(std::string& buffer, std::streamsize count) {
        DWORD bytesWritten{};
        if (!WriteFile(m_pipeHndl.get(),
            buffer.data(),
            static_cast<DWORD>(std::clamp<std::streamsize>(count, 0, std::numeric_limits<DWORD>::max())),
            &bytesWritten,
            NULL))
            throw pipe_exception("WriteFile() encountered an error when writing to pipe");
    }
};

struct ReadThreadCmdQueue {
    std::queue<std::string> queue;
    std::mutex mutex;
    std::condition_variable cv;
    bool bCmdRead{ false };
    static constexpr char exitCmd[2]{ "e" };


    void  writeCmd(const std::string& cmd)
    {
        {
            std::lock_guard lock(mutex);
            queue.push(cmd);
        }
        cv.notify_one();
    }

    void writeCmdAndWaitForRead(const std::string& cmd) {
        writeCmd(cmd);
        waitForCmdToBeRead();
    }

    void writeExitCmd()
    {
        writeCmd(exitCmd);
    }

    void writeExitCmdAndWaitForRead() {
        writeExitCmd();
        waitForCmdToBeRead();
    }

    [[nodiscard]] std::string waitForAndReadCmd()
    {


        std::unique_lock<std::mutex> lock(mutex);
        if (!queue.size())
            cv.wait(lock);
        std::string cmd{ std::move(queue.front()) };
        queue.pop();
        bCmdRead = true;
        lock.unlock();

        cv.notify_one();
        return cmd;
    }

    void waitForCmdToBeRead()
    {
        std::unique_lock lock(mutex);
        cv.wait(lock, [&]() { return bCmdRead; });
        bCmdRead = false;
    }
};

void writeToConsole(InputPipe& inPipe, bool& bExit);

void readFromConsole(OutputPipe& outPipe, ReadThreadCmdQueue& cmdQueue);

enum class SIZE_POS_ARGS: unsigned long long {
    SIZE_X = 0,
    SIZE_Y = 1,
    POS_X = 2,
    POS_Y = 3,
    TOTAL
};
constexpr unsigned long long getArgNum(SIZE_POS_ARGS arg) { return static_cast<unsigned long long>(arg); }

int main(int argc, char* argv[])
{
    // lines to stop execution on entry to wait for debugger attach
    // uncomment when running a debugger and attaching since you can't launch this with a debugger
    // as it requires arguments which need to come form a parent program with win32 Pipes.

    //  /*
    while (!IsDebuggerPresent())
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::cout << "Debugger Present" << std::endl;
    // */

    try {


        /* args expected:
            1: cmd pipe read handle
            2: write to console pipe, read handle
            3: read from console pipe, write handle
            4: size x
            5: size y
            6: pos x
            7: pos y
        */
        if (argc != 7)
            throw std::runtime_error("invalid number of arguments passed to console process");

        InputPipe parentCmdIn(std::move(Win32Helpers::Hndl(reinterpret_cast<HANDLE>(std::stoll(argv[0])))));
        InputPipe consoleWriteIn(std::move(Win32Helpers::Hndl(reinterpret_cast<HANDLE>(std::stoll(argv[1])))));
        OutputPipe consoleReadOut(std::move(Win32Helpers::Hndl(reinterpret_cast<HANDLE>(std::stoll(argv[2])))));


        // the numbering in this code is all wrong!
        int sizeAndPosArgs[getArgNum(SIZE_POS_ARGS::TOTAL)]{ 0 };

        for (int i{ 3 }; i < 7; ++i)
        {
            if (argv[i][0] == 'n')
            {
                argv[i][0] = '0';
                sizeAndPosArgs[i - 3] = -(std::stoi(argv[i]));
            }
            else
                sizeAndPosArgs[i - 3] = (std::stoi(argv[i]));
        }

        if (HWND consoleWindowHndl{ GetConsoleWindow() }; consoleWindowHndl)
        {
            if (!SetWindowPos(consoleWindowHndl,
                HWND_TOP,
                sizeAndPosArgs[getArgNum(SIZE_POS_ARGS::POS_X)],
                sizeAndPosArgs[getArgNum(SIZE_POS_ARGS::POS_Y)],
                sizeAndPosArgs[getArgNum(SIZE_POS_ARGS::SIZE_X)],
                sizeAndPosArgs[getArgNum(SIZE_POS_ARGS::SIZE_Y)],
                SWP_NOZORDER))
                throw std::runtime_error("could not position and size console window, size or position parameters may be invalid");
        }
        else
            throw std::runtime_error("could not retrieve handle to console window");



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
        do {
            std::string cmdBuff{};
            //if read thread exited itself
            if (readThreadFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready
                || writeThreadFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
            {
                bExit = true;
            }
            parentCmdIn.read(cmdBuff, 1024, '\n');
            if (cmdBuff.at(0) == 'e')
            {
                bExit = true;
            }
            else if (cmdBuff.at(0) == 'r')
            {
                readThreadCmdQueue.writeCmdAndWaitForRead(cmdBuff);
            }
        } while (!bExit);

        bWriteThreadExit = true;
        writeThreadFuture.wait();
        if (readThreadFuture.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
        {
            readThreadCmdQueue.writeExitCmdAndWaitForRead();
        }
        readThreadFuture.wait();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception in console process, what:" << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "unknown exception thrown in console process" << std::endl;
    }
    return 0;
}

void writeToConsole(InputPipe& inPipe, bool& bExit)
{
    std::string buffer;
    try {
        do
        {
            inPipe.read(buffer, 1, '\0');
            std::cout.write(buffer.c_str(), buffer.size());
        } while (!bExit);
    }
    catch (Pipe::pipe_exception& e)
    {
        std::cerr << "pipe_exception thrown: " << e.what() << std::endl;
    }
}

enum class ReadCommandArgsIndex: int {
    E_COUNT = 0,
    E_DELIM = 1,
    E_EXTRACT = 2,
    E_TOTAL
};

void readFromConsole(OutputPipe& outPipe, ReadThreadCmdQueue& cmdQueue)
{
    bool bExit{ false };
    std::string proxyString;
    proxyString.resize(std::numeric_limits<int16_t>::max());
    do {
        std::string cmd{ cmdQueue.waitForAndReadCmd() };
        if (cmd.at(0) == 'e')
        {
            bExit = true;
        }
        else if (cmd.at(0) == 'r')
        {

            /*read parameters:
                int count - the number of characters to extract from the stream
                int delim - the delimiter character, when encountered extraction functions stop extracting and return
                bool extract - indicates whether or not to extract the delimiter character from the stream when it's encountered
            */

            /*
             unformatted input command format:
             u;count;delim;extract;
             might look like:
             u;10;\\n;1;
            */

            static constexpr int COUNT{ static_cast<int>(ReadCommandArgsIndex::E_COUNT) };
            static constexpr int DELIM{ static_cast<int>(ReadCommandArgsIndex::E_DELIM) };
            static constexpr int EXTRACT{ static_cast<int>(ReadCommandArgsIndex::E_EXTRACT) };
            static constexpr int TOTAL{ static_cast<int>(ReadCommandArgsIndex::E_TOTAL) };

            //breaking cmd up by semicolons
            std::string cmdArgs[TOTAL];
            {
                size_t seperatorPos{ 0 }, nextSeperatorPos{ 0 };
                for (int i{ 0 }; i < TOTAL; ++i)
                {
                    seperatorPos = cmd.find_first_of(';');
                    nextSeperatorPos = cmd.find_first_of(';', seperatorPos + 1);
                    if (seperatorPos == cmd.npos || nextSeperatorPos == cmd.npos)
                        throw std::runtime_error("read command 'g' was not followed by enough arguments, or formatting of ';' was incorrect");
                    cmdArgs[i] = cmd.substr(seperatorPos + 1, nextSeperatorPos - (seperatorPos + 1));
                    cmd.erase(seperatorPos, nextSeperatorPos - seperatorPos);
                }
            }
            std::streamsize count{ std::stoll(cmdArgs[COUNT]) };

            //make sure count is not larger than the size of the proxy string
            //otherwise the get() function will attempt to write input to an out of range address
            if (count > static_cast<std::streamsize>(proxyString.size()))
                count = static_cast<std::streamsize>(proxyString.size());

            char delim{};
            //if the parent wanted a newline delim they needed to escape the backslash so that it didn't invalidate the command message
            if (cmdArgs[DELIM].compare("\\n") == 0)
                delim = '\n';
            else if (cmdArgs[DELIM].compare("\\0") == 0)
                delim = '\0';
            else
                delim = cmdArgs[DELIM].at(0);

            bool bExtract{ static_cast<bool>(cmdArgs[EXTRACT].at(0) - 48) };

            if (bExtract)
                std::cin.getline(proxyString.data(), count, delim);
            else
                std::cin.get(proxyString.data(), count, delim);


            //we need to append the delimiter to what we output as it was not appended to proxyString by either get() or getline()
            //otherwise when the matching function that this code is replicating is called in the parent it will not be able to find the delimiter it's looking for
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

            //close the out stream on unrecoverable extraction failures from console cin
            //this way a get() function on the parent end will return -1, which is replicant behaviour of a std::istream
            if (std::cin.rdstate() != std::cin.goodbit)
            {
                if (std::cin.bad() | std::cin.eof())
                {
                    bExit = true;
                    //outStream.~WinPipe_StdStreamWrapper(); // why did i do this? do i need to do the same with the outPipe objects
                }
                //or clear the failbit, this might be needed if the get()/getline() funcitons extracted nothing, because they immediately encountered the delim
                else {
                    std::cin.clear();
                }
            }
            if (!bExit)
            {
                outPipe.write(proxyString, count);
            }
        }
    } while (!bExit);
}
