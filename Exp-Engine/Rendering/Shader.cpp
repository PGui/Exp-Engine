#include "Shader.h"

namespace Exp
{

	Shader::Shader(std::string name, std::string vsCode, std::string fsCode, std::string vsfile, std::string fsfile) :
		name(name)
	{
		vertexFilePath = vsfile;
		fragmentFilePath = fsfile;

		Compile(vsCode, fsCode);
	}

	Shader::~Shader()
	{
	}

	void Shader::Use()
	{
		glUseProgram(id);
	}

	void Shader::DeleteProgram()
	{
		glDeleteProgram(id);
	}

	void Shader::SetBool(const std::string & name, bool value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
	}

	void Shader::SetInt(const std::string & name, int value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	}

	void Shader::SetFloat(const std::string & name, float value) const
	{
		glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	}

	void Shader::SetVec2(const std::string & name, const glm::vec2 & value) const
	{
		glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}

	void Shader::SetVec2(const std::string & name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
	}

	void Shader::SetVec3(const std::string & name, const glm::vec3 & value) const
	{
		glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}

	void Shader::SetVec3(const std::string & name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
	}

	void Shader::SetVec4(const std::string & name, const glm::vec3 & value) const
	{
		glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}

	void Shader::SetMat2(const std::string & name, const glm::mat2 & mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void Shader::SetMat3(const std::string & name, const glm::mat3 & mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void Shader::SetMat4(const std::string & name, const glm::mat4 & mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void Shader::Compile(std::string vsCode, std::string fsCode)
	{
		const char* vShaderCode = vsCode.c_str();
		const char* fShaderCode = fsCode.c_str();

		//Compile shaders
		unsigned int vertex, fragment;
		//Vertex
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, nullptr);
		glCompileShader(vertex);
		CheckCompileErrors(vertex, "VERTEX");

		//Fragment
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, nullptr);
		glCompileShader(fragment);
		CheckCompileErrors(fragment, "FRAGMENT");

		//Link shaders
		id = glCreateProgram();
		glAttachShader(id, vertex);
		glAttachShader(id, fragment);
		glLinkProgram(id);
		CheckCompileErrors(id, "PROGRAM");

		//Delete shaders
		glDeleteShader(vertex);
		glDeleteShader(fragment);

		//Query uniforms and attributes
		int nrAttributes, nrUniforms;
		glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &nrAttributes);
		glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &nrUniforms);
		attributes.resize(nrAttributes);
		uniforms.resize(nrUniforms);

		char buffer[128];
		//Iterate over attributes
		for (int i = 0; i < nrAttributes; ++i)
		{
			GLenum glType;
			glGetActiveAttrib(id, i, sizeof(buffer), 0, &attributes[i].Size, &glType, buffer);
			attributes[i].name = std::string(buffer);
			attributes[i].Location = glGetAttribLocation(id, buffer);
		}

		//TODO create location map for each uniforms
		// iterate over all active uniforms
		for (int i = 0; i < nrUniforms; ++i)
		{
			GLenum glType;
			glGetActiveUniform(id, i, sizeof(buffer), 0, &uniforms[i].Size, &glType, buffer);
			uniforms[i].name = std::string(buffer);
			uniforms[i].Location = glGetUniformLocation(id, buffer);
		}
	}

	void Shader::CheckCompileErrors(unsigned int shader, std::string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				spdlog::error("SHADER_COMPILATION_ERROR of type {}\n{}", type, infoLog);
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				spdlog::error("PROGRAM_LINKING_ERROR of type {}\n{}", type, infoLog);			
			}
		}
	}
}
