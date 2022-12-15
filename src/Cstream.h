#include <iostream>
#include <WinConsoles.h>

namespace WinConsoles {
    class Cstream::Cstream_Impl {
    private:
        using Cstream = Cstream_Impl;
    public:
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
    };
}