#pragma once
#include <string>

namespace Exp
{
	class WindowParameters
	{
	public:
		unsigned int screenWidth = 1280;
		unsigned int screenHeight = 768;

		std::string title = "Exp Engine";
	};

	// inline: one shared instance across translation units (was one copy per .cpp).
	inline WindowParameters WinParameters;
}