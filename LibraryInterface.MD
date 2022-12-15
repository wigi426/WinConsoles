# Library Interface

## draft #1

Interface Classes:

    class Console;
    class Cistream;
    class Costream;

Console class:

    Console construction parameters:
        sizeX, sizeY, posX, posY, name, bAutoClose

    Console interface functions:
        WinConsoles::Cistream getIn();
        WinConsoles::Costream getOut();
        void closeConsole();

Cistream class:

    replicates std::istream functions which will be useful to lib users

Costream class:

    replicates sted::ostream functions which will be useful to lib users
