#include "oldSimpleLogger.h"

namespace SimpleLogger
{

	bool Logger::Init()
	{
		if (sm_PipeHandles = std::make_unique<PipeHandles>(PipeHandles::PIPE_DIRECTION::O); !sm_PipeHandles.get()->bInit)
		{
			return false;
		}
		if (sm_LoggerProcess = std::make_unique<LoggerProcess>(*(sm_PipeHandles.get())); !sm_LoggerProcess.get()->bInit)
		{
			return false;
		}
		bInit = true;
		return true;
	}

	void Logger::Log(const std::string &logMessage)
	{
		if (!bInit)
		{
			return;
		}
		DWORD bytesWritten{};
		std::string message = logMessage;
		if (message.back() != '\n')
			message.append(1, '\n');
		DWORD bytesToWrite{};
		if (constexpr DWORD max = (std::numeric_limits<DWORD>::max)(); message.size() > max)
		{
			bytesToWrite = max;
		}
		else
		{
			bytesToWrite = static_cast<DWORD>(message.size());
		}
		if (!WriteFile(sm_PipeHandles.get()->hPipeWrite.handle, message.c_str(), bytesToWrite, &bytesWritten, NULL))
		{
			CreateSystemErrorBox(L"failed to write to pipe going to logger.exe");
		}
		if (bytesWritten != message.size())
		{
			CreateErrorBox(L"Writefile did not write expected amount of bytes to logger");
		}
	}

	Logger::PipeHandles::PipeHandles(const PIPE_DIRECTION &p)
	{
		WinSTRUCT<SECURITY_ATTRIBUTES> sa;
		sa->bInheritHandle = true;
		sa->lpSecurityDescriptor = NULL;
		sa->nLength = sizeof(sa);
		if (!CreatePipe(&hPipeRead.handle, &hPipeWrite.handle, &sa, sizeof(sa)))
		{
			hPipeWrite.handle = hPipeWrite.invalidVal;
			hPipeRead.handle = hPipeRead.invalidVal;
			Logger::CreateSystemErrorBox(L"CreatePipe() failed");
			return;
		}
		if (p == PIPE_DIRECTION::O)
		{
			if (!SetHandleInformation(hPipeWrite.handle, HANDLE_FLAG_INHERIT, 0))
			{
				Logger::CreateSystemErrorBox(L"SetHandleInformation() failed");
				return;
			}
		}
		else
		{
			if (!SetHandleInformation(hPipeRead.handle, HANDLE_FLAG_INHERIT, 0))
			{
				Logger::CreateSystemErrorBox(L"SetHandleInformation() failed");
				return;
			}
		}
		bInit = true;
	}

	Logger::LoggerProcess::LoggerProcess(const PipeHandles &pipe)

	{
		// check pipe was initalized correctly
		if (!pipe.bInit)
		{
			CreateErrorBox(L"pipe passed into LoggerProcess constructor is not initalized");
			return;
		}
		// check if logger.exe exists if not then compile it in ./SimpleLogger/ directory
		if (!std::filesystem::exists("SimpleLogger"))
		{
			if (!std::filesystem::create_directory("SimpleLogger"))
			{
				CreateErrorBox(L"failed to create SimpleLogger directory");
			}
		}
		std::ifstream loggerExeCheck;
		loggerExeCheck.open("SimpleLogger/logger.exe");
		if (!loggerExeCheck.is_open())
		{
			// compile logger
			std::ofstream loggerDotCpp("SimpleLogger/logger.cpp", std::ios_base::out);
			loggerDotCpp.seekp(0);
			if (!loggerDotCpp.is_open())
			{
				CreateErrorBox(L"Issue openeing logger.cpp");
			}
			loggerDotCpp.write(LOGGER_DOT_CPP_CONTENT.c_str(), LOGGER_DOT_CPP_CONTENT.size());
			loggerDotCpp.close();
			WinSTRUCT<SECURITY_ATTRIBUTES> sa;
			sa->bInheritHandle = true;
			sa->lpSecurityDescriptor = NULL;
			sa->nLength = sizeof(sa);
			WinHANDLE<INVALID_HANDLE_VALUE> hCompileOutput;
			hCompileOutput.handle = CreateFile(L"SimpleLogger/compileOutput.txt", GENERIC_WRITE, FILE_SHARE_WRITE, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hCompileOutput.handle == hCompileOutput.invalidVal)
			{
				CreateSystemErrorBox(L"CreateFile failed to create/get a handle to SimpleLogger/compileOutput.txt");
			}
			WinSTRUCT<STARTUPINFO> si;
			si->dwFlags |= STARTF_USESTDHANDLES;
			si->hStdError = hCompileOutput.handle;
			si->cb = sizeof(sa);
			WinSTRUCT<PROCESS_INFORMATION> pi{};
			std::wstring cmdLine{L"g++ -static -static-libgcc -static-libstdc++ SimpleLogger/logger.cpp -o SimpleLogger/logger.exe"};
			if (!CreateProcess(
					NULL,
					cmdLine.data(),
					NULL,
					NULL,
					true,
					0,
					NULL,
					NULL,
					&si,
					&pi))
			{
				CreateSystemErrorBox(L"Failed to Create process g++ for logger.exe compilation");
				return;
			}
			DWORD waitResult = WaitForSingleObject(pi->hProcess, 10000);
			if (waitResult == WAIT_TIMEOUT)
			{
				CreateErrorBox(L"Timed out waiting for g++");
				return;
			}
			else if (waitResult == WAIT_FAILED)
			{
				CreateSystemErrorBox(L"WaitForSingleObject returned WAIT_FAILED when waiting for g++");
				return;
			}
			// confirm logger.exe exists
			loggerExeCheck.open("SimpleLogger/logger.exe");
			if (!loggerExeCheck.is_open())
			{
				CreateErrorBox(L"unknown problem compiling logger.exe, check SimpleLogger/compileOutput.txt");
				return;
			}
			loggerExeCheck.close();
		}
		else
		{
			loggerExeCheck.close();
		}

		// run logger
		WinSTRUCT<STARTUPINFO> sa;
		sa->dwFlags |= STARTF_USESTDHANDLES;
		sa->hStdInput = sm_PipeHandles.get()->hPipeRead.handle;
		sa->cb = sizeof(sa);
		if (!CreateProcess(L"SimpleLogger/logger.exe",
						   NULL,
						   NULL,
						   NULL,
						   TRUE,
						   CREATE_NEW_CONSOLE,
						   NULL,
						   NULL,
						   &sa,
						   &piLogger))
		{
			CreateSystemErrorBox(L"CreateProcess() failed to create logger.exe");
			return;
		}
		bInit = true;
	}
	Logger::LoggerProcess::~LoggerProcess()
	{
		Log("exit");
		if (WAIT_TIMEOUT == WaitForSingleObject(piLogger->hProcess, 2000))
			TerminateProcess(piLogger->hProcess, 1);
	}

	template <>
	WinSTRUCT<PROCESS_INFORMATION>::~WinSTRUCT()
	{
		if (structure.hProcess != INVALID_HANDLE_VALUE)
			CloseHandle(structure.hProcess);
		if (structure.hThread != INVALID_HANDLE_VALUE)
			CloseHandle(structure.hThread);
	}

	void Logger::CreateSystemErrorBoxW(const std::wstring_view &contextMessage, const int line, const char *file, const bool systemErr)
	{
		std::wstringstream fullMessage{};
		if (systemErr)
		{
			LPTSTR buff = NULL;
			DWORD sysErrCode = GetLastError();
			assert(FormatMessage(
					   FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					   NULL,
					   sysErrCode,
					   0,
					   (LPTSTR)&buff,
					   0,
					   NULL) &&
				   "FormatMessageA() for System error in SimpleLogger failed");
			fullMessage << "System Error in Simple Logger, context: \n\""
						<< contextMessage << "\""
						<< '\n'
						<< "LINE: " << line << '\n'
						<< "FILE: " << file << '\n'
						<< "\nSystem Error: 0x"
						<< std::hex
						<< sysErrCode
						<< ":\n"
						<< buff;
		}
		else
		{
			fullMessage << "General Error in Simple Logger, context: \n\""
						<< contextMessage << "\""
						<< '\n'
						<< "LINE: " << line << '\n'
						<< "FILE: " << file;
		}
		MessageBox(NULL, fullMessage.str().c_str(), NULL, MB_OK);
	}
};
