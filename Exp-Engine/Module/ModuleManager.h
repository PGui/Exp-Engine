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
		std::map<unsigned int, IModuleInterface *> m_modules;
		std::vector<unsigned int> m_insertOrder;

		IModuleInterface *GetModulePtr(const std::string ModuleName)
		{
			unsigned int moduleId = SID(ModuleName);
			return m_modules.find(moduleId) != m_modules.end() ? m_modules[moduleId] : nullptr;
		}

	public:
		ModuleManager();
		~ModuleManager();

		template <class T>
		void AddModule(const std::string moduleName)
		{
			unsigned int moduleId = SID(moduleName);

			if (m_modules.find(moduleId) == m_modules.end())
			{
				m_insertOrder.push_back(moduleId);
				IModuleInterface * newModule = new T();
				m_modules[moduleId] = newModule;
			}
		}

		template<typename T>
		inline T* GetModule(const std::string ModuleName)
		{
			return static_cast<T*>(GetModulePtr(ModuleName));
		}

		void InitializeModules();

		void ShutdownModules();
	};
}


