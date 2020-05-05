#pragma once
#include "../Helpers/Singleton.h"
namespace Exp
{
	class IModuleInterface
	{
	private:

	public:
		IModuleInterface() {};
		virtual ~IModuleInterface() {};

		virtual void StartUp() = 0;

		virtual void Shutdown() = 0;

		virtual void DisplayUI();

		// Called after all modules StartUp were called.
		virtual void PostInitialize();
	};
}
