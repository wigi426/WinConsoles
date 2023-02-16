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
        using int_type = std::istream::int_type;
        using char_type = std::istream::char_type;

        //unformatted input functions
        int_type get();
        Cistream& get(char_type& ch);
        Cistream& get(char_type* s, std::streamsize count);
        Cistream& get(char_type* s, std::streamsize count, char_type delim);
        Cistream& get(std::streambuf& strbuf);
        Cistream& get(std::streambuf& strbuf, char_type delim);

        int_type peek();

        Cistream& unget();

        Cistream& putback(char_type ch);

        Cistream& getline(char_type* s, std::streamsize count);
        Cistream& getline(char_type* s, std::streamsize count, char_type delim);

        Cistream& ignore(std::streamsize count = 1, int_type delim = std::istream::traits_type::eof());

        Cistream& read(char_type* s, std::streamsize count);

        std::streamsize readsome(char_type* s, std::streamsize count);

        std::streamsize gcount() const;

        //formatted input function template
        template<typename T>
        Cistream& operator>>(T& value);

    protected:
        Cistream();

    private:
        Cistream(const Cistream&) = delete;
        Cistream(Cistream&&) = delete;

        std::unique_ptr<Cistream_Impl> Cistream_pImpl;
    };

    class Costream: public Cstream {
    public:
        using char_type = std::ostream::char_type;
        using traits_type = std::ostream::traits_type;

        Costream& put(char_type ch);
        Costream& write(const char_type* s, std::streamsize count);

        Costream& operator<<(short value);
        Costream& operator<<(unsigned short value);
        Costream& operator<<(int value);
        Costream& operator<<(unsigned int value);
        Costream& operator<<(long value);
        Costream& operator<<(unsigned long value);
        Costream& operator<<(long long value);
        Costream& operator<<(unsigned long long value);
        Costream& operator<<(float value);
        Costream& operator<<(double value);
        Costream& operator<<(long double value);
        Costream& operator<<(bool value);
        Costream& operator<<(const void* value);
        Costream& operator<<(const volatile void* value);
        Costream& operator<<(std::nullptr_t);
        Costream& operator<<(std::basic_streambuf<char_type, traits_type>* sb);

        Costream& operator<<(
            std::ios_base& (*func)(std::ios_base&));

        Costream& operator<<(
            std::basic_ios<char_type, traits_type>& (*func)(std::basic_ios<char_type, traits_type>&));

        Costream& operator<<(
            std::basic_ostream<char_type, traits_type>& (*func)(std::basic_ostream<char_type, traits_type>&));

        Costream& flush();

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