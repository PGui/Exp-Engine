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

	void RenderingModule::Render()
	{
		rmt_ScopedCPUSample(Rendering, 0)

		m_geometryCommands.sort();

		// nothing to pass as GL doesn't have any contexts
		m_geometryCommands.submit(nullptr, true);
	}

}

