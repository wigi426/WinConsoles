#pragma once
namespace WinConsoles {
    class Console_Impl {
    private:
        using Console = Console_Impl;
    public:
        Console(
            const std::string& name,
            int sizeX,
            int sizeY,
            int posX,
            int posY,
            bool bAutoClose = true);
        Costream& getOut();
        Cistream& getIn();
        void closeConsole();

        Console(Console&&);

        ~Console();
    }
}