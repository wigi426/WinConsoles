#pragma once
#include <string>
#include <memory>


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

    private:
        Cstream() = delete;
        Cstream(const Cstream&) = delete;
        Cstream(Cstream&&) = delete;
        class Cstream_Impl;
        std::unique_ptr<Cstream_Impl> pImpl;
    }
    class Cistream: public Cstream {
    public:
        using int_type = std::istream::int_type;
        using char_type = std::istream::char_type;

        //unformatted input functions
        int_type get();
        Cistream& get(char_type& ch);
        Cistream& get(char_type* s, std::streamsize count);
        Cistream& get(char_type* s, std::streamsize count, char_type delim);
        Cistream& get(basic_streambuf& strbuf);
        Cistream& get(basic_streambuf& strbuf, char_type delim);

        int_type peek();

        Cistream& unget();

        Cistream& putback(char_type ch);

        Cistream& getline(char_type* s, std::streamsize count);
        Cistream& getline(char_type* s, std::streamsize count, char_type delim);

        Cistream& ignore(std::streamsize count = 1, int_type delim = Traits::eof());

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
        class Cistream_Impl;
        Cistream_Impl* pImpl;
    };

    class Costream {
    public:
        using char_type = std::ostream::char_type;

        Costream& put(char_type ch);
        basic_ostream& write(const char_type* s, std::streamsize count);

        template<typename T>
        Cistream& operator<<(T& value);

        Costream& flush();

    protected:
        Costream();
    private:
        Costream(const Costream&) = delete;
        Costream(Costream&&) = delete;

        class Costream_Impl;
        Costream_Impl* pImpl;
    };
    class Console {
    public:
        Console(
            const std::string& name = "Extra Console",
            int sizeX = 100,
            int sizeY = 100,
            int posX = 0,
            int posY = 0,
            bool bAutoClose = true);
        Costream& getOut();
        Cistream& getIn();
        void closeConsole();

        //provide move constructor

    private:
        Console(const Console&);
        class Console_Impl;
        Console_Impl* pImpl;
    };
};