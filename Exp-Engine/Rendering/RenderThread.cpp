#include "RenderThread.h"
#include <Remotery/Remotery.h>

namespace Exp
{
	FTL_THREAD_FUNC_DECL RenderThreadStart(void *arg) 
	{
		Exp::ThreadArgs *renderArgs = (Exp::ThreadArgs *)arg;
		GLFWwindow * mainWindow = renderArgs->mainWindow;
		GLFWwindow * slaveWindow = renderArgs->slaveWindow;
		Exp::GameSyncState * syncState = renderArgs->syncState;
		delete renderArgs;

		rmt_SetCurrentThreadName("Render");

		// Bind ourselves to the last hardware thread
		// Logic will use [0, NumHardwareThreads() - 1)
		ftl::SetCurrentThreadAffinity(ftl::GetNumHardwareThreads() - 1);

		glfwMakeContextCurrent(slaveWindow);

		while (!syncState->shouldQuit.load(std::memory_order_relaxed)) 
		{
			rmt_ScopedCPUSample(RenderLoop, 0);


			rmt_BeginCPUSample(WaitForNewScene, 0);
			uint64 SceneNumber = syncState->syncQueue.Pop();
			rmt_EndCPUSample();

			std::cout << "Starting rendering... "<< SceneNumber << std::endl;
			rmt_BeginCPUSample(DrawFrame, 0);
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
			rmt_EndCPUSample();

			//End loop
			std::cout << "Rendering done..." << SceneNumber << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(0));
		}

		return 0;
	}
}

