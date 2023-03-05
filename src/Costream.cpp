#include "Costream.h"
#include <utility>


Cout::Cout(Win32Helpers::Hndl& writePipeHndl, Win32Helpers::Hndl& cmdPipeHndl):
    m_writePipeHndl{ writePipeHndl }, m_cmdPipeHndl{ cmdPipeHndl } {}

void Cout::write(const std::string& content)
{
    //send cmd message to write
    //send content
}