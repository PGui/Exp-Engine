#pragma once
#include <string>

#define SID(string) CustomSimpleHash(string)

namespace Exp
{
	inline unsigned int CustomSimpleHash(const std::string &str)
	{
		unsigned long hash = 5381;

		for (auto& it : str)
			hash = ((hash << 5) + hash) + static_cast<char>(it);

		return hash;
	}

	inline unsigned int CustomSimpleHash(const char* cStr)
	{
		std::string str(cStr);
		return CustomSimpleHash(str);
	}
}