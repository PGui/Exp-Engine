#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <spdlog/spdlog.h>

#include "ModuleInterface.h"

#include "../hash/Hash.h"
#include "../helpers/Singleton.h"

namespace Exp
{
	/*
		Owns every engine module. Modules are started, displayed and shut down
		in insertion order (shutdown in reverse), so dependencies stay predictable.
	*/
	class ModuleManager :
		public Singleton<ModuleManager>
	{
		friend class Singleton<ModuleManager>;

	private:
		// Insertion order is the source of truth; the map only accelerates lookups.
		std::vector<std::unique_ptr<IModuleInterface>> modules;
		std::unordered_map<unsigned int, IModuleInterface*> lookup;

		IModuleInterface* GetModulePtr(const std::string& moduleName) const
		{
			auto it = lookup.find(SID(moduleName));
			return it != lookup.end() ? it->second : nullptr;
		}

	public:
		ModuleManager() = default;
		~ModuleManager() = default;

		template <class T, class... Args>
		T* AddModule(const std::string& moduleName, Args&&... args)
		{
			static_assert(std::is_base_of_v<IModuleInterface, T>, "Modules must derive from IModuleInterface");

			const unsigned int moduleId = SID(moduleName);
			if (auto it = lookup.find(moduleId); it != lookup.end())
			{
				spdlog::error("Module '{}' is already registered", moduleName);
				return dynamic_cast<T*>(it->second);
			}

			auto module = std::make_unique<T>(std::forward<Args>(args)...);
			T* raw = module.get();
			lookup[moduleId] = raw;
			modules.push_back(std::move(module));
			return raw;
		}

		template<typename T>
		T* GetModule(const std::string& moduleName) const
		{
			return static_cast<T*>(GetModulePtr(moduleName));
		}

		void InitializeModules();

		void ShutdownModules();

		void DisplayModulesUI();
	};
}
