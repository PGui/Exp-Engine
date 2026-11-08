#include "CrashHandler.h"

#ifdef _WIN32

#include <cstdio>
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

namespace Exp
{
	static LONG WINAPI UnhandledExceptionHandler(EXCEPTION_POINTERS* info)
	{
		const DWORD code = info->ExceptionRecord->ExceptionCode;
		void* address = info->ExceptionRecord->ExceptionAddress;
		std::fprintf(stderr, "\n===== CRASH: exception 0x%08lX at %p =====\n", code, address);
		if (code == EXCEPTION_ACCESS_VIOLATION && info->ExceptionRecord->NumberParameters >= 2)
		{
			std::fprintf(stderr, "Access violation %s address 0x%p\n",
				info->ExceptionRecord->ExceptionInformation[0] ? "writing" : "reading",
				reinterpret_cast<void*>(info->ExceptionRecord->ExceptionInformation[1]));
		}

		HANDLE process = GetCurrentProcess();
		HANDLE thread = GetCurrentThread();
		SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
		SymInitialize(process, nullptr, TRUE);

		CONTEXT context = *info->ContextRecord;
		STACKFRAME64 frame = {};
		frame.AddrPC.Offset = context.Rip;
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrFrame.Offset = context.Rbp;
		frame.AddrFrame.Mode = AddrModeFlat;
		frame.AddrStack.Offset = context.Rsp;
		frame.AddrStack.Mode = AddrModeFlat;

		alignas(SYMBOL_INFO) char symbolBuffer[sizeof(SYMBOL_INFO) + 512];
		for (int i = 0; i < 48; ++i)
		{
			if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, process, thread, &frame, &context,
				nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr) || frame.AddrPC.Offset == 0)
				break;

			SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			symbol->MaxNameLen = 511;
			DWORD64 displacement = 0;
			const char* name = SymFromAddr(process, frame.AddrPC.Offset, &displacement, symbol) ? symbol->Name : "?";

			IMAGEHLP_LINE64 line = {};
			line.SizeOfStruct = sizeof(line);
			DWORD lineDisplacement = 0;
			if (SymGetLineFromAddr64(process, frame.AddrPC.Offset, &lineDisplacement, &line))
				std::fprintf(stderr, "  #%02d %s  (%s:%lu)\n", i, name, line.FileName, line.LineNumber);
			else
				std::fprintf(stderr, "  #%02d %s  [0x%llx]\n", i, name, static_cast<unsigned long long>(frame.AddrPC.Offset));
		}
		std::fflush(stderr);
		SymCleanup(process);
		return EXCEPTION_EXECUTE_HANDLER; // terminate without the Windows error dialog
	}

	void InstallCrashHandler()
	{
		SetUnhandledExceptionFilter(UnhandledExceptionHandler);
	}
}

#else

namespace Exp
{
	void InstallCrashHandler() {}
}

#endif
