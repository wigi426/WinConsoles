#pragma once
#include "Cstream.h"

namespace WinConsoles {
    class Costream::Costream_Impl: public Cstream::Cstream_Impl {
    private:

        using char_type = std::ostream::char_type;

        Costream_Impl& put(char_type ch);
        Costream_Impl& write(const char_type* s, std::streamsize count);

        template<typename T>
        Costream_Impl& operator<<(T& value);

        Costream_Impl& flush();

    private:
        Costream_Impl(const Costream_Impl&) = delete;
        Costream_Impl(Costream_Impl&&) = delete;
    };
};
