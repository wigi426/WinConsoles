#pragma once
#include <string>
#include <memory>

namespace WinConsoles
{
    class Console_Impl;

    class Console {
    public:

        /*
        Constructs a new Console object:
            name - The name that will be used for the console window title
            sizeX - the horizontal size of the console window
            sizeY - the vertical size of the console window
            posX - the horizontal position* of the console window
            posY - the vertical position* of the console window
                *posX and posY positions are relative to the top left of the primary display
            bAutoClose - poorly defined parameter FIXME (leave default)
        */
        Console(
            const std::string& name = "WinConsoles Extra Console",
            int sizeX = 100,
            int sizeY = 100,
            int posX = 0,
            int posY = 0,
            bool bAutoClose = true);

        /*
        Writes a string to the Console Window
            buffer - the string containing characters to be written to the console window
        */
        void write(const std::string& buffer);

        /*
        Reads from the Console window, Waits for "count" characters to be read, or for the delim to be read
            buffer - out parameter in which read characters are placed
            count - the number of characters to be read
            delim - the delimiter which if encountered will stop reading
            storeDelim - determines whether the delim is kept or discarded. It will be removed from the console input buffer either way
        */
        void read(std::string& buffer, const std::streamsize count, const char delim, const bool storeDelim);

        /*
        Close the console window, not required for cleanup, but can be called prematurely if desired.
        */
        void closeConsole();

        ~Console();
        Console(Console&&);
    private:
        Console(const Console&) = delete;
        std::unique_ptr<Console_Impl> pImpl;
    };
};