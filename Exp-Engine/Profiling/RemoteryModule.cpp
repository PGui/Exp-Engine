#include "RemoteryModule.h"
#include <iostream>


namespace Exp
{
	void RemoteryModule::StartUp()
	{
		std::cout << "	void RemoteryModule::StartUp() Interface ctor" << std::endl;
		rmt_CreateGlobalInstance(&rmt);
	}
	void RemoteryModule::Shutdown()
	{
		std::cout << "	void RemoteryModule::StartUp() Interface dtor" << std::endl;
		rmt_DestroyGlobalInstance(rmt);
	}
	void RemoteryModule::Print()
	{
		std::cout << "Here is the remotery module." << std::endl;
	}
}
