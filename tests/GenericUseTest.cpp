#include <iostream>
#include <string>
#include <stdexcept>
#include <WinConsoles.h>

using WinConsoles::Console;

int main()
{

    // due to the external nature of the results of utilising this library, the results of tests need to be observed by humans and cannot be automated
    try {
        std::string buffer{"Hello World!\n"};

        Console testConsole("test", 500, 300, -1000, 300, false);

        testConsole.write(buffer);
        testConsole.read(buffer, buffer.size(), '\n', false);
        std::cout << "read from console: " << buffer << std::endl;
        testConsole.read(buffer, buffer.size(), '\n', false);
        std::cout << "read from console: " << buffer << std::endl;
        testConsole.closeConsole();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    catch (const std::exception& e)
    {
        std::cerr << "exception caught in test: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "unhandeld exception" << std::endl;
    }



    return 0;
}