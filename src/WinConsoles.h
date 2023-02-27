#pragma once
#include <string>
#include <memory>
#include <ios>
#include <istream>
#include <ostream>
#include <streambuf>

class Cstream_Impl;
class Cistream_Impl;
class Costream_Impl;
class Console_Impl;


//Front end interface for library
namespace WinConsoles
{
    class Cstream {
        //state functions
        bool good() const;
        bool eof() const;
        bool fail() const;
        bool bad() const;
        bool operator!() const;
        operator void* () const;
        explicit operator bool() const;
        std::ios_base::iostate rdstate() const;
        void setstate(std::ios_base::iostate state);
        void clear(std::ios_base::iostate state = std::ios_base::goodbit);
    protected:
        std::unique_ptr<Cstream_Impl> Cstream_pImpl;
    private:
        Cstream() = delete;
        Cstream(const Cstream&) = delete;
        Cstream(Cstream&&) = delete;
    };

    class Cistream: public Cstream {
    public:
        Cistream& read(std::string buffer, std::streamsize count, char delim);
        Cistream& read(char* buffer, std::streamsize count, char delim);

    protected:
        Cistream();

    private:
        Cistream(const Cistream&) = delete;
        Cistream(Cistream&&) = delete;

        std::unique_ptr<Cistream_Impl> Cistream_pImpl;
    };

    class Costream: public Cstream {
    public:
        Costream& write(std::string buffer);
        Costream& write(const char* buffer, std::streamsize size);

    protected:
        Costream();
    private:
        Costream(const Costream&) = delete;
        Costream(Costream&&) = delete;


        std::unique_ptr<Costream_Impl> Costream_pImpl;
    };
    class Console {
    public:
        Console(
            const std::string& name = "WinConsoles Extra Console",
            int sizeX = 100,
            int sizeY = 100,
            int posX = 0,
            int posY = 0,
            bool bAutoClose = true);
        Costream& getOut() const;
        Cistream& getIn() const;
        void closeConsole();

        ~Console();
        //provide move constructor
    private:
        Console(const Console&);
        std::unique_ptr<Console_Impl> Console_pImpl;
    };
};