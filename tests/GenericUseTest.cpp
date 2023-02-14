#include <iostream>
#include <string>
#include <WinConsoles.h>

using WinConsoles::Console;

int main()
{
    Console testConsole("test", 50, 150, 500, 300, true);
    testConsole.getOut() << "Hello World enter some input" << std::endl;
    std::string line{};
    line.resize(10);
    testConsole.getIn().getline(line.data(), line.size());
    std::cout << "line taken from other console: " << line << std::endl;
    std::cout << "press enter to close other console..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    testConsole.closeConsole();
    std::cout << "press enter to close this test..." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return 0;
}