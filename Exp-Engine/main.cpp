#include "Module/ModuleManager.h"
#include "Core/EngineModule.h"
#include "Rendering/RenderingModule.h"
#include "MaterialLibrary/MaterialLibraryModule.h"
#include "Profiling/RemoteryModule.h"
#include "Input/InputModule.h"


void MainJob(ftl::TaskScheduler * taskScheduler, void * arg)
{
	//Add modules
	Exp::ModuleManager::Get().AddModule<Exp::RemoteryModule>("Remotery");
	
	Exp::ModuleManager::Get().AddModule<Exp::EngineModule>("Engine");
	Exp::ModuleManager::Get().AddModule<Exp::RenderingModule>("Rendering");
	Exp::ModuleManager::Get().AddModule<Exp::MaterialLibraryModule>("MaterialLibrary");
	Exp::ModuleManager::Get().AddModule<Exp::InputModule>("Input");
	//Initialize them
	Exp::ModuleManager::Get().InitializeModules();

	//Launch main loop
	Exp::EngineModule * EngineModule = Exp::ModuleManager::Get().GetModule<Exp::EngineModule>("Engine");
	EngineModule->taskScheduler = taskScheduler;
	EngineModule->RunEngine();

	Exp::ModuleManager::Get().ShutdownModules();
}

int main()
{
	ftl::TaskScheduler taskScheduler;
	taskScheduler.Run(512, MainJob, nullptr, ftl::GetNumHardwareThreads(), ftl::EmptyQueueBehavior::Sleep);

	return 0;
}