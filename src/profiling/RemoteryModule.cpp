#include "RemoteryModule.h"
#include <iostream>
#include <spdlog/spdlog.h>


namespace Exp
{
	void RemoteryModule::StartUp()
	{
		rmt_CreateGlobalInstance(&rmt);

		spdlog::info("RemoteryModule StartUp");
	}
	void RemoteryModule::Shutdown()
	{
		rmt_DestroyGlobalInstance(rmt);

		spdlog::info("RemoteryModule Shutdown");
		
	}
}
