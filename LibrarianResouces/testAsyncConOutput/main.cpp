#include <iostream>
#include <string>
#include <thread>
#include <future>


void write() {
    int c{};
    while (true)
    {

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Hello#" << c << '\n';
        c++;
    }
    return;
}
void read() {

    std::string buff{};
    while (true)
    {
        std::getline(std::cin, buff);
        std::cout << "input: " << buff << '\n';
    }
    return;
}

int main()
{
    auto writeFuture = std::async(std::launch::async, write);
    auto readFuture = std::async(std::launch::async, read);

    writeFuture.wait();
    readFuture.wait();

    return 0;
}