#include "theLib.hpp"

namespace theLib
{
    inline const static std::wstring ProgDir{L"RuntimeCompiledProg"};
    inline const static std::wstring SourceDir{L"src"};
    inline const static std::wstring BuildDir{L"build"};

    void BuildProgram()
    {
        if (!std::filesystem::exists(ProgDir))
        {
            std::filesystem::create_directory(ProgDir);
        }
        if (!std::filesystem::exists(ProgDir + L'/' + SourceDir))
        {
            std::filesystem::create_directory(ProgDir + L'/' + SourceDir);
        }
        if (!std::filesystem::exists(ProgDir + L'/' + BuildDir))
        {
            std::filesystem::create_directory(ProgDir + L'/' + BuildDir);
        }
        // create directory for source and build output (try using boost instead of std::filesystem)

        //^^or test if it's ok to compile this in c++17 then compile

        // the program that uses the lib in c++11 or 14 or somethig

        std::ofstream CMakeListsFile;
        std::wstring FileName{L"CMakeLists.txt"};
        CMakeListsFile.open(ProgDir + L'/' + FileName, std::fstream::out);
        assert(CMakeListsFile.is_open());
        // create cmakelists file

        CMakeListsFile.write(CMAKE_LISTS_CONTENT.c_str(), CMAKE_LISTS_CONTENT.size());
        // load content into file#
        CMakeListsFile.close();

        std::ofstream someProgCppFiles;
        FileName = L"someProg.cpp";
        someProgCppFiles.open(ProgDir + L'/' + SourceDir + L'/' + FileName, std::fstream::out);
        assert(someProgCppFiles.is_open());
        // create someprog.cpp file

        someProgCppFiles.write(SOME_PROG_CPP_CONTENT.c_str(), SOME_PROG_CPP_CONTENT.size());
        // load content into file
        someProgCppFiles.close();

        STARTUPINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(pi));
        std::wstring cmd;
        cmd.append(L"cmake -B");
        cmd.append(ProgDir + L'/' + BuildDir);
        cmd.append(L" -S");
        cmd.append(ProgDir + L'/' + SourceDir);
        assert(CreateProcess(NULL, cmd.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi));
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        // run cmake in directory

        cmd.clear();
        cmd.append(L"cmake --build ");
        cmd.append(ProgDir + L'/' + BuildDir);
        assert(CreateProcess(NULL, cmd.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi));
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        // run cmake --build in directory
    }

    void RunProgram()
    {
        // run the previously built program
    }

};