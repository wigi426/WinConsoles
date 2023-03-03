# Library Interface

Interface Classes:

    class Console;
    class Cistream;
    class Costream;

Console class:

    Console construction parameters:
        sizeX, sizeY, posX, posY, name, bAutoClose

    Console interface functions:
        void write(const std::string& buffer);
        void read( std::string& buffer, std::streamsize count, char delim)
        void closeConsole();
