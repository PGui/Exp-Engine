#include "RenderingModule.h"
#include <iostream>
#include <Remotery/Remotery.h>

namespace Exp
{
	RenderingModule::RenderingModule()
	{
	}


	RenderingModule::~RenderingModule()
	{
	}

	void RenderingModule::StartUp()
	{
		rmt_ScopedCPUSample(RenderingModuleStartUp, 0)
		std::cout << "RenderingModule StartUp" << std::endl;
	}

	void RenderingModule::Shutdown()
	{
		std::cout << "RenderingModule Shutdown" << std::endl;
	}
}

