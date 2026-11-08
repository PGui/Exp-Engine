#include "ModuleManager.h"
#include "imgui.h"

namespace Exp
{
	void ModuleManager::InitializeModules()
	{
		for (auto& module : modules)
		{
			module->StartUp();
		}

		for (auto& module : modules)
		{
			module->PostInitialize();
		}
	}

	void ModuleManager::ShutdownModules()
	{
		// Shutdown in reverse order, then destroy in reverse order as well.
		for (auto it = modules.rbegin(); it != modules.rend(); ++it)
		{
			(*it)->Shutdown();
		}

		while (!modules.empty())
		{
			modules.pop_back();
		}
		lookup.clear();
	}

	void ModuleManager::DisplayModulesUI()
	{
		ImGui::Begin("Exp Engine");

		for (auto& module : modules)
		{
			module->DisplayUI();
		}

		ImGui::End();
	}
}
