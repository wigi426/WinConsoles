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

    replicates std::istream functions, except for those which expose the stream buffer (suchs as rdbuf())

Costream class:

    replicates sted::ostream fucntions, except for those which expose the stream buffer (suchs as rdbuf())
