#pragma once
#include <string>

#define SID(string) custom_simple_hash(string)

namespace Exp
{
	inline unsigned int custom_simple_hash(const std::string &str)
	{
		unsigned long hash = 5381;

		for (auto& it : str)
			hash = ((hash << 5) + hash) + static_cast<char>(it);

		return hash;
	}

	inline unsigned int custom_simple_hash(const char* cStr)
	{
		std::string str(cStr);
		return custom_simple_hash(str);
	}
}

