#pragma once
#include "module/ModuleInterface.h"
#include <entt/entt.hpp>

namespace Exp
{
	class ECSModule :
		public IModuleInterface
	{
	public:
		entt::registry registry;
	public:
		virtual void StartUp() override;

		virtual void Shutdown() override;

		virtual void DisplayUI() override;

		void Print();
	};
}