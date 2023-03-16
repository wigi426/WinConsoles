#include <iostream>
#include <string>
#include <WinConsoles.h>

using WinConsoles::Console;

int main()
{
    /*
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
*/

    try {
        //new interface use example:
        std::string buffer{"Hello World!\n"};
        Console testConsole("test", 50, 150, 500, 300, true);
        //new winConsoles console should now be open
        testConsole.write(buffer);
        //winConsoles console should receive a line of "Hello World!"
        testConsole.read(buffer, buffer.size(), '\n');
        std::cout << buffer << std::endl;
        //winConsoles console should read a line of input from the user and then output in 
        //the default console window
        testConsole.closeConsole();
        //the winConsoles console should now be closed.
    }
    catch (std::exception& e)
    {
        std::cerr << "exception caught in test: " << e.what() << std::endl;
    }



    return 0;
}