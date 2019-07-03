#include "RenderingModule.h"
#include <iostream>

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
		std::cout << "RenderingModule StartUp" << std::endl;
	}

	void RenderingModule::Shutdown()
	{
		std::cout << "RenderingModule Shutdown" << std::endl;
	}

}

