#pragma once
#include "Cstream.h"
#include <streambuf>


class Cistream_Impl: public Cstream_Impl {
private:
public:
    using int_type = std::istream::int_type;
    using char_type = std::istream::char_type;

    //unformatted input functions
    int_type get();
    Cistream_Impl& get(char_type& ch);
    Cistream_Impl& get(char_type* s, std::streamsize count);
    Cistream_Impl& get(char_type* s, std::streamsize count, char_type delim);
    Cistream_Impl& get(std::streambuf& strbuf);
    Cistream_Impl& get(std::streambuf& strbuf, char_type delim);

    int_type peek();

    Cistream_Impl& unget();

    Cistream_Impl& putback(char_type ch);

    Cistream_Impl& getline(char_type* s, std::streamsize count);
    Cistream_Impl& getline(char_type* s, std::streamsize count, char_type delim);

    Cistream_Impl& ignore(std::streamsize count = 1, int_type delim = std::istream::traits_type::eof());

    Cistream_Impl& read(char_type* s, std::streamsize count);

    std::streamsize readsome(char_type* s, std::streamsize count);

    std::streamsize gcount() const;

    //formatted input function template
    template<typename T>
    Cistream_Impl& operator>>(T& value);

private:
    Cistream_Impl(const Cistream_Impl&) = delete;
    Cistream_Impl(Cistream_Impl&&) = delete;

};
