#include "module/ModuleManager.h"
#include "core/EngineModule.h"
#include "rendering/RenderingModule.h"
#include "materialLibrary/MaterialLibraryModule.h"
#include "profiling/RemoteryModule.h"
#include "input/InputModule.h"
#include "ECS/ECSModule.h"
//#include <ftl/task_scheduler.h>

void MainJob(/*ftl::TaskScheduler * taskScheduler, void * arg*/)
{
	//Add modules
	Exp::ModuleManager::Get().AddModule<Exp::RemoteryModule>("Remotery");

	Exp::ModuleManager::Get().AddModule<Exp::EngineModule>("Engine");
	Exp::ModuleManager::Get().AddModule<Exp::RenderingModule>("Rendering");
	Exp::ModuleManager::Get().AddModule<Exp::MaterialLibraryModule>("MaterialLibrary");
	Exp::ModuleManager::Get().AddModule<Exp::InputModule>("Input");
	Exp::ModuleManager::Get().AddModule<Exp::ECSModule>("ECS");

	//Initialize them
	Exp::ModuleManager::Get().InitializeModules();

	//Launch main loop
	Exp::EngineModule * EngineModule = Exp::ModuleManager::Get().GetModule<Exp::EngineModule>("Engine");
	//EngineModule->taskScheduler = taskScheduler;
	EngineModule->RunEngine();

	Exp::ModuleManager::Get().ShutdownModules();
}

int main()
{
	//ftl::TaskScheduler taskScheduler;
	//taskScheduler.Run(512, MainJob, nullptr, ftl::GetNumHardwareThreads(), ftl::EmptyQueueBehavior::Sleep);
	MainJob();
	return 0;
}
