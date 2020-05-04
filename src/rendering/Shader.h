#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <spdlog/spdlog.h>


#include "ShadingTypes.h"

#include <glm/glm.hpp>
#include "imgui.h"

#include <glad/glad.h>

namespace Exp
{
	class Shader
	{

	public:
		unsigned int id = -1;
		std::string name;
		Shader() {};
		Shader(std::string name, std::string vsCode, std::string fsCode, std::string vsfile, std::string fsfile);

		~Shader();

		void Use();
		void DeleteProgram();

		void SetBool(const std::string & name, bool value) const;
		void SetInt(const std::string & name, int value) const;
		void SetFloat(const std::string & name, float value) const;
		void SetVec2(const std::string & name, const glm::vec2 & value) const;
		void SetVec2(const std::string & name, float x, float y) const;
		void SetVec3(const std::string & name, const glm::vec3 & value) const;
		void SetVec3(const std::string & name, float x, float y, float z) const;
		void SetVec4(const std::string & name, const glm::vec3 & value) const;
		void SetMat2(const std::string & name, const glm::mat2 & value) const;
		void SetMat3(const std::string & name, const glm::mat3 & value) const;
		void SetMat4(const std::string & name, const glm::mat4 & value) const;

		std::string vertexFilePath;
		std::string fragmentFilePath;

	private:

		std::vector<Uniform> uniforms;
		std::vector<VertexAttribute> attributes;

		void Compile(std::string vsCode, std::string fsCode);

		void CheckCompileErrors(unsigned int shader, std::string type);
	};

}
