#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include <Windows.h>


std::queue<char> writeCmdQueue;
std::condition_variable writeCmdQueue_cv;
std::mutex writeCmdQueue_mutex;
bool cmdReceived = false;

void writeToConsole(const HANDLE inPipe);
void readFromConsole(HANDLE outPipe, std::queue<char>& cmdQueue, std::condition_variable& cmdQueue_cv, std::mutex& cmdQueue_mutex);

//cmd line expects three HANDLE values: 1: readEnd for cmd pipe 
                                    //  2: readEnd for consoleWritePipe
                                    //  3: wrtieEnd for ConsoleReadPipe
int main(int argc, char* argv[])
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    assert(argc == 3 && "incorrect number of args passed into ConsoleProcess");

    HANDLE cmdPipeRead{ nullptr };
    HANDLE consoleWritePipeRead{ nullptr };
    HANDLE consoleReadPipeWrite{ nullptr };

    cmdPipeRead = reinterpret_cast<HANDLE>(std::stoll(argv[0]));
    consoleWritePipeRead = reinterpret_cast<HANDLE>(std::stoll(argv[1]));
    consoleReadPipeWrite = reinterpret_cast<HANDLE>(std::stoll(argv[2]));

    auto writeThreadFuture = std::async(std::launch::async, writeToConsole, consoleWritePipeRead);

    bool bExit{ false };
    char cmd{};
    do {
        assert(ReadFile(cmdPipeRead, &cmd, 1, NULL, NULL));
        if (cmd == 'e')
        {
            {
                std::lock_guard<std::mutex> lock(writeCmdQueue_mutex);
                writeCmdQueue.push('e');
            }
            writeCmdQueue_cv.notify_all();
            assert(writeThreadFuture.wait_for(std::chrono::seconds(1)) == std::future_status::ready);
            bExit = true;
        }
        else if (cmd == 'w')
        {
            {
                std::lock_guard<std::mutex> lock(writeCmdQueue_mutex);
                writeCmdQueue.push('w');
            }
            writeCmdQueue_cv.notify_one();
            {
                std::unique_lock<std::mutex> lock(writeCmdQueue_mutex);
                writeCmdQueue_cv.wait(lock, [] {return cmdReceived;});
            }
        }
    } while (!bExit);

    CloseHandle(cmdPipeRead);
    CloseHandle(consoleWritePipeRead);
    CloseHandle(consoleReadPipeWrite);
    return 0;

}


void writeToConsole(const HANDLE inPipe)
{
    try {
        bool bExit{ false };
        std::string buff{};
        buff.resize(std::numeric_limits<uint16_t>::max());
        char cmd{};
        do {
            std::unique_lock<std::mutex> lock(writeCmdQueue_mutex);
            if (!writeCmdQueue.size())
                writeCmdQueue_cv.wait(lock);
            cmd = writeCmdQueue.front();
            writeCmdQueue.pop();
            cmdReceived = true;
            lock.unlock();
            writeCmdQueue_cv.notify_one();
            if (cmd == 'e')
            {
                bExit = true;
            }
            else if (cmd == 'w')
            {
                DWORD read;
                assert(ReadFile(inPipe, buff.data(), static_cast<DWORD>(buff.size()), &read, NULL));
                std::cout << std::to_string(read) << std::endl;
                std::cout << buff.substr(0, read) << std::endl;;
            }
        } while (!bExit);
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    std::cout << "exiting write thread" << std::endl;
    return;
}
