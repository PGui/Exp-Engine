#include "ShaderLoader.h"
#include "../Rendering/Shader.h"

namespace Exp
{
	Shader ShaderLoader::LoadShader(std::string name, std::string vsFile, std::string fsFile)
	{
		//Read files
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		std::string vertexCode;
		std::string fragmentCode;

		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			vShaderFile.open(vsFile);
			fShaderFile.open(fsFile);

			std::stringstream vShaderStream, fShaderStream;

			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			spdlog::error("SHADER::FILE_NOT_SUCCESFULLY_READ. {}/{}", vsFile, fsFile);
		}

		return Shader(name, vertexCode, fragmentCode, vsFile, fsFile);
	}
}