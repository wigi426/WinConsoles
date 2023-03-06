#include <iostream>
#include <string>
#include <cassert>
int main()
{

    while (true)
    {
        char buff[500];


        std::cin.get(buff, 500, '\0');
        std::cout << buff << '\n';
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return 0;
}