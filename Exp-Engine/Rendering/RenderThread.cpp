#include "RenderThread.h"

namespace Exp
{
	FTL_THREAD_FUNC_DECL RenderThreadStart(void *arg) 
	{

		Exp::ThreadArgs *renderArgs = (Exp::ThreadArgs *)arg;
		GLFWwindow * mainWindow = renderArgs->mainWindow;
		GLFWwindow * slaveWindow = renderArgs->slaveWindow;
		Exp::GameSyncState * syncState = renderArgs->syncState;
		delete renderArgs;

		// Bind ourselves to the last hardware thread
		// Logic will use [0, NumHardwareThreads() - 1)
		ftl::SetCurrentThreadAffinity(ftl::GetNumHardwareThreads() - 1);

		glfwMakeContextCurrent(slaveWindow);

		while (!syncState->shouldQuit.load(std::memory_order_relaxed)) {

			syncState->syncSempahore.Wait();

			std::cout << "Starting rendering..." << std::endl;

			std::this_thread::sleep_for(std::chrono::milliseconds(5));

			std::cout << "Rendering done..." << std::endl;
		}

		return 0;
	}
}

