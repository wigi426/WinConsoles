#include <iostream>
#include <string>
#include <Windows.h>



int main(int argc, char* argv[]) {

    //takes in args which represent sequences of charactres
    // which if passed in as a line to the stdin given at CreateProcess call
    // will cause the console to perform certain actions instead of outputing that 
    // string which is the regular behviour
    std::string inBuff{};
    std::string outBuff{};
    std::istream in;
    std::ostream out;
    in.rdbuf(std::cin.rdbuf());
    out.rdbuf(std::cout.rdbuf());
    AllocConsole();
    do {
        in.read(inBuff.data(), inBuff.max_size())
            if (inBuff)
    }

}