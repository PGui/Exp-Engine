#include "CrashHandler.h"

#ifdef _WIN32

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <crtdbg.h>
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

namespace Exp
{
	static void PrintStack(CONTEXT context)
	{
		HANDLE process = GetCurrentProcess();
		HANDLE thread = GetCurrentThread();
		SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
		SymInitialize(process, nullptr, TRUE);

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
	}

	static void PrintCurrentStack()
	{
		CONTEXT context = {};
		RtlCaptureContext(&context);
		PrintStack(context);
	}

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
		PrintStack(*info->ContextRecord);
		return EXCEPTION_EXECUTE_HANDLER; // terminate without the Windows error dialog
	}

	static void TerminateHandler()
	{
		std::fprintf(stderr, "\n===== CRASH: std::terminate =====\n");
		if (std::exception_ptr current = std::current_exception())
		{
			try { std::rethrow_exception(current); }
			catch (const std::exception& e) { std::fprintf(stderr, "Uncaught exception: %s\n", e.what()); }
			catch (...) { std::fprintf(stderr, "Uncaught non-standard exception\n"); }
		}
		PrintCurrentStack();
		std::_Exit(3);
	}

	static void AbortHandler(int)
	{
		std::fprintf(stderr, "\n===== CRASH: abort() =====\n");
		PrintCurrentStack();
		std::_Exit(3);
	}

	// Debug CRT assertions/errors (e.g. checked iterators): print instead of a blocking dialog.
	static int CrtReportHook(int reportType, char* message, int* returnValue)
	{
		std::fprintf(stderr, "\n===== CRT %s: %s\n", reportType == _CRT_ASSERT ? "ASSERT" : "ERROR", message ? message : "");
		PrintCurrentStack();
		if (returnValue)
			*returnValue = 0;
		std::_Exit(3);
	}

	void InstallCrashHandler()
	{
		SetUnhandledExceptionFilter(UnhandledExceptionHandler);
		std::set_terminate(TerminateHandler);
		std::signal(SIGABRT, AbortHandler);
		_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
		_CrtSetReportHook(CrtReportHook);
	}
}

#else

namespace Exp
{
	void InstallCrashHandler() {}
}

#endif
