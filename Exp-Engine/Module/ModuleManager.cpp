#include "ModuleManager.h"

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
		for (auto it = m_insertOrder.begin(); it != m_insertOrder.end(); ++it)
		{
			m_modules[*it]->StartUp();
		}
	}

	void ModuleManager::ShutdownModules()
	{
		// Shutdown in reverse order
		for (auto it = m_insertOrder.rbegin(); it != m_insertOrder.rend(); ++it)
		{
			m_modules[*it]->Shutdown();
		}

		//Delete the modules.
		for (auto it = m_insertOrder.begin(); it != m_insertOrder.end(); ++it)
		{
			delete m_modules[*it];
		}
	}

}

