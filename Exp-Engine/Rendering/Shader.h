#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <spdlog/spdlog.h>


#include "ShadingTypes.h"

#include "../glm/glm.hpp"
#include "../imgui/imgui.h"

#include <glad/glad.h>

namespace Exp
{
	class Shader
	{

	public:
		unsigned int ID;
		std::string Name;
		Shader() {};
		Shader(std::string name, std::string vsCode, std::string fsCode, std::string vsfile, std::string fsfile);

		~Shader();

		void use();
		void deleteProgram();

		void setBool(const std::string & name, bool value) const;
		void setInt(const std::string & name, int value) const;
		void setFloat(const std::string & name, float value) const;
		void setVec2(const std::string & name, const glm::vec2 & value) const;
		void setVec2(const std::string & name, float x, float y) const;
		void setVec3(const std::string & name, const glm::vec3 & value) const;
		void setVec3(const std::string & name, float x, float y, float z) const;
		void setVec4(const std::string & name, const glm::vec3 & value) const;
		void setMat2(const std::string & name, const glm::mat2 & value) const;
		void setMat3(const std::string & name, const glm::mat3 & value) const;
		void setMat4(const std::string & name, const glm::mat4 & value) const;

		std::string VertexFilePath;
		std::string FragmentFilePath;

	private:

		std::vector<Uniform> uniforms;
		std::vector<VertexAttribute> Attributes;

		void compile(std::string vsCode, std::string fsCode);

		void checkCompileErrors(unsigned int shader, std::string type);
	};

}
