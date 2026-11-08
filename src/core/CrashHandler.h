#pragma once

namespace Exp
{
	// Installs a handler that logs the exception and a symbolized call stack
	// when the process crashes (Windows only; no-op elsewhere).
	void InstallCrashHandler();
}
