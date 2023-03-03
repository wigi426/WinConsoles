#pragma once
#include <streambuf>
#include <string>
#include <Windows.h>
#include "WinConsoles.h"

class Cin {
public:
    Cin(HANDLE pipeHndl);
private:
    Cin(const Cin&) = delete;
    Cin(Cin&&) = delete;
};
