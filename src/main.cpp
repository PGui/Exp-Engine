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
#include "core/CrashHandler.h"

int main(int argc, char** argv)
{
	Exp::InstallCrashHandler();

	// Command line:
	//   --frames N          quit after N frames (automated runs / CI smoke tests)
	//   --vsync on|off      vertical sync (default on)
	//   --max-fps N         frame cap when vsync is off (0 = uncapped)
	//   --sim fixed|variable  simulation timestep mode (default fixed + interpolation)
	//   --fixed-rate HZ     fixed simulation rate (default 60)
	int maxFrames = 0;
	const char* vsyncArg = nullptr;
	const char* simArg = nullptr;
	int maxFps = -1;
	int fixedRate = 0;
	for (int i = 1; i + 1 < argc; ++i)
	{
		if (std::strcmp(argv[i], "--frames") == 0)          maxFrames = std::atoi(argv[++i]);
		else if (std::strcmp(argv[i], "--vsync") == 0)      vsyncArg = argv[++i];
		else if (std::strcmp(argv[i], "--max-fps") == 0)    maxFps = std::atoi(argv[++i]);
		else if (std::strcmp(argv[i], "--sim") == 0)        simArg = argv[++i];
		else if (std::strcmp(argv[i], "--fixed-rate") == 0) fixedRate = std::atoi(argv[++i]);
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

	if (vsyncArg)
		engine->SetVSync(std::strcmp(vsyncArg, "off") != 0);
	if (maxFps >= 0)
		engine->maxFps = maxFps;
	if (simArg && std::strcmp(simArg, "variable") == 0)
		engine->simulationMode = Exp::EngineModule::SimulationMode::Variable;
	if (fixedRate > 0)
		engine->updatePeriod = 1.0 / fixedRate;

	{
		Exp::DemoApplication app;
		engine->RunEngine(app, maxFrames);
	}

	modules.ShutdownModules();
	return 0;
}
