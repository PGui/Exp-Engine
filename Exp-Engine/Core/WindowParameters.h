#pragma once
#include <string>

namespace Exp
{
	struct WindowParameters
	{
		unsigned int screenWidth = 1280;
		unsigned int screenHeight = 768;

		std::string title = "Exp Engine";
	};

	static Exp::WindowParameters WinParameters;
}