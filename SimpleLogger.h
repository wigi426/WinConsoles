#pragma once
#include			  <memory>
#include			  <string>
#include			 <cassert>
#include			 <sstream>
#include		  <filesystem>
#include			 <fstream>
#include			  <limits>
#include		   <algorithm>
#include		   <Windows.h>
#include	  <namedpipeapi.h>
#include <processthreadsapi.h>

namespace SimpleLogger {

	//the value of N = what to treat as the invalid handle value
	//either NULL or INVALID_HANDLE_VALUE
	template<HANDLE INVALID_VAL>
	struct WinHANDLE {
		HANDLE handle{ INVALID_VAL };
		constexpr static HANDLE invalidVal{ INVALID_VAL };
		~WinHANDLE()
		{
			if (handle != INVALID_VAL)
				CloseHandle(handle);
		}
	};

	template<typename T>
	struct WinSTRUCT {
		static_assert(std::is_same<T, PROCESS_INFORMATION>::value ||
			          std::is_same<T, STARTUPINFO>::value ||
			          std::is_same<T, SECURITY_ATTRIBUTES>::value,
						"WinSTRUCT of this type is invalid");
		T  structure;
		T* operator->() { return &structure; } //how tf does this work
		T& operator* () { return structure; }
		T* operator& () { return &structure; }
		WinSTRUCT<T> () { ZeroMemory(&structure, sizeof(structure)); }
		~WinSTRUCT<T>() {};
	};

	template<>
	WinSTRUCT<PROCESS_INFORMATION>::~WinSTRUCT();

	class Logger {
	public:
		//if returns false, logger is not usable, suggest asserting/throwing/returning from main
		static bool Init();
		static void Log(const std::string& logMessage);
	private:
		struct PipeHandles;
		struct LoggerProcess;

		//for log to check, if false Log() will do nothing
		inline static bool bInit{ false };

		inline static std::unique_ptr<PipeHandles>   sm_PipeHandles{ nullptr };
		inline static std::unique_ptr<LoggerProcess> sm_LoggerProcess{ nullptr };

		struct PipeHandles {
			enum class PIPE_DIRECTION {
				O,//writing out
				I //reading in
			};

			WinHANDLE<INVALID_HANDLE_VALUE> hPipeWrite{};
			WinHANDLE<INVALID_HANDLE_VALUE> hPipeRead{};
			bool bInit{ false };

			PipeHandles(const PIPE_DIRECTION& p);

			PipeHandles(const PipeHandles&) = delete;
			PipeHandles(PipeHandles&&)      = delete;
		};

		struct LoggerProcess {
			WinSTRUCT<PROCESS_INFORMATION>piLogger{};
			bool bInit{ false };

			LoggerProcess(const PipeHandles& pipe);
			~LoggerProcess();

			LoggerProcess(const LoggerProcess&) = delete;
			LoggerProcess(LoggerProcess&&)      = delete;
		};

		//Creates a Win32 message box with the last error recievved from windows with 
		//GetLastError, contextMessage should give information on where the failure occured
		//message box blocks execution until ok button is pressed
		//macro copies line and file into function call
#define CreateSystemErrorBox(contextMessage) CreateSystemErrorBoxW(contextMessage, __LINE__, __FILE__, true)
		//extra macro to call function without system error output, just a dialog box with ok button which will include the context message and file/line values
#define CreateErrorBox(contextMessage) CreateSystemErrorBoxW(contextMessage, __LINE__, __FILE__, false)
		static void CreateSystemErrorBoxW(const std::wstring_view& contextMessage, const int line, const char* file, const bool systemErr = true);

		//logger.cpp content:
		inline static const std::string LOGGER_DOT_CPP_CONTENT{
		R"DELIM(
#include <iostream>
#include <string>

int main() {
	std::string input;
		do {
			std::getline(std::cin, input);
			std::cout << input << '\n';
		}while (input != "exit");
	return 0;
}
)DELIM"
		};
	};
};




