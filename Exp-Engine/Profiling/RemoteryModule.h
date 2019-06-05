#pragma once
#include "../Module/ModuleInterface.h"
#include <Remotery/Remotery.h>

namespace Exp
{
	class RemoteryModule : 
		public IModuleInterface
	{
	protected:
		Remotery* rmt;

	public:
		RemoteryModule() {};
		virtual ~RemoteryModule() {};

		virtual void StartUp();

		virtual void Shutdown();

		void Print();
	};
}

