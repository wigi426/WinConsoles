#pragma once
#include <iostream>
#include "WinConsoles.h"

namespace WinConsoles {
    class Cstream::Cstream_Impl {
    private:

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
        Cstream_Impl() = delete;
        Cstream_Impl(const Cstream&) = delete;
        Cstream_Impl(Cstream&&) = delete;
    };
}