#include <Cstream.h>
#include <streambuf>

namespace WinConsoles {
    class Cistream_Impl: public Cstream {
    private:
        using Cistream = Cistream_Impl;
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

    private:
        Cistream(const Cistream&) = delete;
        Cistream(Cistream&&) = delete;

    }
};