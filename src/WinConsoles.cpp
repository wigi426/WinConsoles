#include "WinConsoles.h"
#include "Cstream.h"
#include "Costream.h"
#include "Cistream.h"
#include "Console.h"

//connecting front end interface to implementations
namespace WinConsoles
{

#pragma region Cstream
    bool Cstream::good() const
    {
        return Cstream_pImpl->good();
    }
    bool Cstream::eof() const
    {
        return Cstream_pImpl->eof();
    }
    bool Cstream::fail() const
    {
        return Cstream_pImpl->fail();
    }
    bool Cstream::bad() const
    {
        return Cstream_pImpl->bad();
    }
    bool Cstream::operator!() const
    {
        return Cstream_pImpl->operator!();
    }
    Cstream::operator void* () const
    {
        return Cstream_pImpl->operator void* ();
    }
    Cstream::operator bool() const
    {
        return Cstream_pImpl->operator bool();
    }
    std::ios_base::iostate Cstream::rdstate() const
    {
        return Cstream_pImpl->rdstate();
    }
    void Cstream::setstate(std::ios_base::iostate state)
    {
        Cstream_pImpl->setstate(state);
    }
    void Cstream::clear(std::ios_base::iostate state)
    {
        Cstream_pImpl->clear(state);
    }
#pragma endregion
#pragma region Cistream

    Cistream::int_type Cistream::get()
    {
        return Cistream_pImpl.get()->get();
    }
    Cistream& Cistream::get(char_type& ch)
    {
        Cistream_pImpl.get()->get(ch);
        return *this;
    }
    Cistream& Cistream::get(char_type* s, std::streamsize count)
    {
        Cistream_pImpl.get()->get(s, count);
        return *this;
    }
    Cistream& Cistream::get(char_type* s, std::streamsize count, char_type delim)
    {
        Cistream_pImpl.get()->get(s, count, delim);
        return *this;
    }
    Cistream& Cistream::get(std::streambuf& strbuf)
    {
        Cistream_pImpl.get()->get(strbuf);
        return *this;
    }
    Cistream& Cistream::get(std::streambuf& strbuf, char_type delim)
    {
        Cistream_pImpl.get()->get(strbuf, delim);
        return *this;
    }

    Cistream::int_type Cistream::peek()
    {
        return Cistream_pImpl.get()->peek();
    }

    Cistream& Cistream::unget()
    {
        Cistream_pImpl.get()->unget();
        return *this;
    }

    Cistream& Cistream::putback(char_type ch)
    {
        Cistream_pImpl.get()->putback(ch);
        return *this;
    }

    Cistream& Cistream::getline(char_type* s, std::streamsize count)
    {
        Cistream_pImpl.get()->getline(s, count);
        return *this;
    }

    Cistream& Cistream::getline(char_type* s, std::streamsize count, char_type delim)
    {
        Cistream_pImpl.get()->getline(s, count, delim);
        return *this;
    }

    Cistream& Cistream::ignore(std::streamsize count, int_type delim)
    {
        Cistream_pImpl.get()->ignore(count, delim);
        return *this;
    }

    Cistream& Cistream::read(char_type* s, std::streamsize count)
    {
        Cistream_pImpl.get()->read(s, count);
        return *this;
    }

    std::streamsize Cistream::readsome(char_type* s, std::streamsize count)
    {
        return Cistream_pImpl.get()->readsome(s, count);
    }

    std::streamsize Cistream::gcount() const
    {
        return Cistream_pImpl.get()->gcount();
    }

#pragma endregion

#pragma region Costream
    Costream& Costream::put(char_type ch)
    {
        Costream_pImpl.get()->put(ch);
        return *this;
    }

    Costream& Costream::write(const char_type* s, std::streamsize count)
    {
        Costream_pImpl.get()->write(s, count);
        return *this;
    }

    template<typename T>
    Costream& Costream::operator<<(T value)
    {
        Costream_pImpl.get()->operator<<(Costream_pImpl.get(), value);
        return *this;
    }

    Costream& Costream::flush()
    {
        Costream_pImpl.get()->flush();
        return *this;
    }
#pragma endregion

#pragma region Console

    Console::Console(const std::string& name,
        int sizeX,
        int sizeY,
        int posX,
        int posY,
        bool bAutoClose)
    {
        Console_pImpl = std::make_unique<Console_Impl>(name, sizeX, sizeY, posX, posY, bAutoClose);
    }


    Costream& Console::getOut() const
    {
        return Console_pImpl.get()->getOut();
    }

    Cistream& Console::getIn() const
    {
        return Console_pImpl.get()->getIn();
    }

    void Console::closeConsole()
    {
        Console_pImpl.get()->closeConsole();
    }

#pragma endregion





};

