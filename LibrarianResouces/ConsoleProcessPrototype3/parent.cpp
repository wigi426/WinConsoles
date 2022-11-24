#include <iostream>
#include <streambuf>


int main()
{
    cistream stream(std::cin.rdbuf());
    char c;
    stream.get(c);
    stream.ignore(1000, '\n');
    char buff[2];
    stream.get(buff, 2);

    std::cout << buff;

    return 0;
}