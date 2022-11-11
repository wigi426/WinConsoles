#include <iostream>
#include <WinConsoles.h>

using Con = WinConsoles::Console;

int main()
{
    Con::CreateConsole();
    Con::Write("Hello World!", 13);
    std::cin.get();
    return 0;
}