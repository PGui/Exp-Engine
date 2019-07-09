#pragma once
#include <iostream>
#include <string>

namespace Exp
{
	class Shader;

	class ShaderLoader
	{

	public:
		static Shader LoadShader(std::string name, std::string vsFile, std::string fsFile);
	};
}

