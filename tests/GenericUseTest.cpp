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

//new interface use example:
    std::string buffer{"Hello World!\n"};
    Console testConsole("test", 50, 150, 500, 300, true);
    //new winConsoles console should now be open
    testConsole.getOut().write(buffer);
    testConsole.getOut().write(buffer.c_str(), buffer.size());
    //winConsoles console should receive two lines of "Hello World!"
    testConsole.getIn().read(buffer, buffer.size(), '\n');
    std::cout << buffer << std::endl;
    testConsole.getIn().read(buffer.c_str(), buffer.size(), '\n');
    std::cout << buffer << std::endl;
    //winConsole console should read two lines of input from the user and then output them in 
    //the default console window




    return 0;
}