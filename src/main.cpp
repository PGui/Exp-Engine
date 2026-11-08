#include <cstdlib>
#include <cstring>

#include "module/ModuleManager.h"
#include "core/EngineModule.h"
#include "rendering/RenderingModule.h"
#include "materialLibrary/MaterialLibraryModule.h"
#include "profiling/RemoteryModule.h"
#include "input/InputModule.h"
#include "ecs/ECSModule.h"
#include "sandbox/DemoApplication.h"

int main(int argc, char** argv)
{
	// --frames N : quit after N frames (handy for automated runs / CI smoke tests).
	int maxFrames = 0;
	for (int i = 1; i + 1 < argc; ++i)
	{
		if (std::strcmp(argv[i], "--frames") == 0)
			maxFrames = std::atoi(argv[i + 1]);
	}

	Exp::ModuleManager& modules = Exp::ModuleManager::Get();

	// Registration order = startup order (shutdown runs in reverse).
	modules.AddModule<Exp::RemoteryModule>("Remotery");
	Exp::EngineModule* engine = modules.AddModule<Exp::EngineModule>("Engine");
	modules.AddModule<Exp::RenderingModule>("Rendering");
	modules.AddModule<Exp::MaterialLibraryModule>("MaterialLibrary");
	modules.AddModule<Exp::InputModule>("Input");
	modules.AddModule<Exp::ECSModule>("ECS");

	modules.InitializeModules();

	{
		Exp::DemoApplication app;
		engine->RunEngine(app, maxFrames);
	}

	modules.ShutdownModules();
	return 0;
}
