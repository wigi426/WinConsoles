#pragma once
#include <string>
#include <memory>
#include "WinConsoles.h"
#include "Costream.h"
#include "Cistream.h"

namespace WinConsoles {
    class Console_Impl {
    public:
        Console_Impl(
            const std::string& name,
            int sizeX,
            int sizeY,
            int posX,
            int posY,
            bool bAutoClose = true);

        void write(const std::string& buffer);
        void read(std::string& buffer, const std::streamsize count, const char delim, const bool storeDelim);
        void closeConsole();

        Console_Impl(Console_Impl&&);
        ~Console_Impl();

    private:
        std::unique_ptr<Cin> cin;
        std::unique_ptr<Cout> cout;
        std::unique_ptr<Cout> cmdOut;
        std::unique_ptr<ConfirmReceiver> confirmIn;
        bool bClosed{ false };
    };
};