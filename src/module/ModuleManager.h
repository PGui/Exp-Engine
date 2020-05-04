#pragma once
#include <vector>
#include <map>
#include <string>

#include "ModuleInterface.h"

#include "../Hash/Hash.h"
#include "../Helpers/Singleton.h"

namespace Exp
{
	class ModuleManager:
		public Singleton<ModuleManager>
	{
		friend class Singleton<ModuleManager>;

	private:
		std::map<unsigned int, IModuleInterface *> modules;
		std::vector<unsigned int> insertOrder;

		IModuleInterface *GetModulePtr(const std::string moduleName)
		{
			unsigned int moduleId = SID(moduleName);
			return modules.find(moduleId) != modules.end() ? modules[moduleId] : nullptr;
		}

	public:
		ModuleManager();
		~ModuleManager();

		template <class T>
		void AddModule(const std::string moduleName)
		{
			unsigned int moduleId = SID(moduleName);

			if (modules.find(moduleId) == modules.end())
			{
				insertOrder.push_back(moduleId);
				IModuleInterface * newModule = new T();
				modules[moduleId] = newModule;
			}
		}

		template<typename T>
		inline T* GetModule(const std::string ModuleName)
		{
			return static_cast<T*>(GetModulePtr(ModuleName));
		}

		void InitializeModules();

		void ShutdownModules();

		void DisplayModulesUI();
	};
}


