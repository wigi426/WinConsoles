#include <iostream>
#include <streambuf>
#include <fstream>

class myFileBuf : public std::filebuf {
private:
    bool bManualSetEOF{ false };
public:
    void setEOF() {
        // std::filebuf::setp(std::filebuf::epptr(), std::filebuf::epptr());
        setg(eback(), egptr() + 1, egptr());

        bManualSetEOF = true;
    }
    using std::filebuf::basic_filebuf;
    virtual std::streamsize xsgetn(char_type* s, std::streamsize count) override
    {
        if (bManualSetEOF)
            return traits_type::eof();
        else
            return std::filebuf::xsgetn(s, count);
    }
};






int main()
{
    //file stream which takes console input and puts it in the file
    myFileBuf fb;
    fb.open("text.txt", std::ios_base::trunc | std::ios_base::in | std::ios_base::out);
    std::ofstream ofs;
    ofs.set_rdbuf(&fb);
    std::ifstream ifs;
    ifs.set_rdbuf(&fb);

    ofs.seekp(0);
    ofs.write("hello", 6);
    ifs.seekg(0);


    int one = ifs.get();
    // static_cast<myFileBuf*>(ofs.rdbuf())->setEOF();
    //  ofs.seekp(1, std::ios_base::end);
    static_cast<myFileBuf*>(ofs.rdbuf())->setEOF();

    int two = ifs.get();

    std::cout << "one: " << one << "\ttwo: " << two << std::endl;
    if (ifs.eof())
        std::cout << "ifs hit eof\n";
    return 0;
}