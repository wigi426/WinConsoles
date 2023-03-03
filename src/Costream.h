#pragma once
#include <Windows.h>
#include "WinConsoles.h"


class Cout {
public:
    Cout(HANDLE pipeHndl);
private:
    Cout(const Cout&) = delete;
    Cout(Cout&&) = delete;
};

