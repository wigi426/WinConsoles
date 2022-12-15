#include "WinConsoles.h"
#include <Cstream.h>
#include <Costream.h>
#include <Cistream.h>

//connecting front end interface to implementations
namespace WinConsoles
{

#pragma region Cstream
    bool Cstream::good() const
    {
        return pImpl->good();
    }
    bool Cstream::eof() const
    {
        return pImpl->eof();
    }
    bool Cstream::fail() const
    {
        return pImpl->fail();
    }
    bool Cstream::bad() const
    {
        return pImpl->bad();
    }
    bool Cstream::operator!() const
    {
        return pImpl->operator!();
    }
    Cstream::operator void* () const
    {
        return pImpl->operator void* ();
    }
    explicit Cstream::operator bool() const
    {
        return pImpl->operator bool();
    }
    std::ios_base::iostate Cstream::rdstate() const
    {
        return pImpl->rdstate();
    }
    void Cstream::setstate(std::ios_base::iostate state)
    {
        pImpl->setstate(state);
    }
    void Cstream::clear(std::ios_base::iostate state)
    {
        pImpl->clear(state);
    }
#pragma endregion

    Console::Console(const std::string& name,
        int sizeX,
        int sizeY,
        int posX,
        int posY,
        bool bAutoClose)
    {
        ConsoleImpl::ConsoleImpl(name, sizeX, sizeY, posX, posY, bAutoClose);
    }







}

