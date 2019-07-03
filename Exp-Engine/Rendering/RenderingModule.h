#pragma once

#include "../Module/ModuleInterface.h"

namespace Exp
{
	class RenderingModule :
		public IModuleInterface
	{
	public:
		RenderingModule();
		virtual ~RenderingModule();

		virtual void StartUp();
		virtual void Shutdown();
	};
}



