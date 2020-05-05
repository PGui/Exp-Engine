#include "ECSModule.h"
#include <spdlog/spdlog.h>

namespace Exp
{
	void ECSModule::StartUp()
	{
		spdlog::info("ECSModule StartUp");
	}
	void ECSModule::Shutdown()
	{
		spdlog::info("ECSModule Shutdown");
	}

	void ECSModule::DisplayUI()
	{

	}

	void ECSModule::Print()
	{

	}
}
