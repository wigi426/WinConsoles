#include <Cstream.h>

namespace WinConsoles {
    class Costream_Impl: public Cstream {
    private:
        using Costream = Costream_Impl;
        using char_type = std::ostream::char_type;

        Costream& put(char_type ch);
        basic_ostream& write(const char_type* s, std::streamsize count);

        template<typename T>
        Cistream& operator<<(T& value);

        Costream& flush();

    private:
        Costream(const Costream&) = delete;
        Costream(Costream&&) = delete;
    }
};
