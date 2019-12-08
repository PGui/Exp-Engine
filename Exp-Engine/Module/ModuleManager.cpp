#include "ModuleManager.h"
#include "../Imgui/imgui.h"

namespace Exp
{
	ModuleManager::ModuleManager()
	{
	}


	ModuleManager::~ModuleManager()
	{
	}

	void ModuleManager::InitializeModules()
	{
		for (auto it = insertOrder.begin(); it != insertOrder.end(); ++it)
		{
			modules[*it]->StartUp();
		}

		for (auto it = insertOrder.begin(); it != insertOrder.end(); ++it)
		{
			modules[*it]->PostInitialize();
		}
	}

	void ModuleManager::ShutdownModules()
	{
		// Shutdown in reverse order
		for (auto it = insertOrder.rbegin(); it != insertOrder.rend(); ++it)
		{
			modules[*it]->Shutdown();
		}

		//Delete the modules.
		for (auto it = insertOrder.begin(); it != insertOrder.end(); ++it)
		{
			delete modules[*it];
		}
	}

	void ModuleManager::DisplayModulesUI()
	{
		ImGui::Begin("Exp Engine");

		for (auto it = modules.begin(); it != modules.end(); ++it)
		{
			(*it).second->DisplayUI();
		}

		ImGui::End();
	}

}

