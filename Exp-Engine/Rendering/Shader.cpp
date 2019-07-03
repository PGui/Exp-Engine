#include "Shader.h"

namespace Exp
{

	Shader::Shader()
	{
	}

	Shader::Shader(std::string name, std::string vsCode, std::string fsCode) :
		Name(name)
	{
		compile(vsCode, fsCode);
	}

	Shader::~Shader()
	{
	}

	void Shader::use()
	{
		glUseProgram(ID);
	}

	void Shader::reload()
	{
		//glDeleteProgram(ID);
	}

	void Shader::setBool(const std::string & name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void Shader::setInt(const std::string & name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void Shader::setFloat(const std::string & name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void Shader::setVec2(const std::string & name, const glm::vec2 & value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void Shader::setVec2(const std::string & name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}

	void Shader::setVec3(const std::string & name, const glm::vec3 & value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void Shader::setVec3(const std::string & name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}

	void Shader::setVec4(const std::string & name, const glm::vec3 & value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void Shader::setMat2(const std::string & name, const glm::mat2 & mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void Shader::setMat3(const std::string & name, const glm::mat3 & mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void Shader::setMat4(const std::string & name, const glm::mat4 & mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void Shader::compile(std::string vsCode, std::string fsCode)
	{
		const char* vShaderCode = vsCode.c_str();
		const char* fShaderCode = fsCode.c_str();

		//Compile shaders
		unsigned int vertex, fragment;
		//Vertex
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, nullptr);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");

		//Fragment
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, nullptr);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");

		//Link shaders
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");

		//Delete shaders
		glDeleteShader(vertex);
		glDeleteShader(fragment);

		//Query uniforms and attributes
		int nrAttributes, nrUniforms;
		glGetProgramiv(ID, GL_ACTIVE_ATTRIBUTES, &nrAttributes);
		glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &nrUniforms);
		Attributes.resize(nrAttributes);
		Uniforms.resize(nrUniforms);

		char buffer[128];
		//Iterate over attributes
		for (int i = 0; i < nrAttributes; ++i)
		{
			GLenum glType;
			glGetActiveAttrib(ID, i, sizeof(buffer), 0, &Attributes[i].Size, &glType, buffer);
			Attributes[i].Name = std::string(buffer);
			Attributes[i].Location = glGetAttribLocation(ID, buffer);
		}

		//TODO create location map for each uniforms
		// iterate over all active uniforms
		for (int i = 0; i < nrUniforms; ++i)
		{
			GLenum glType;
			glGetActiveUniform(ID, i, sizeof(buffer), 0, &Uniforms[i].Size, &glType, buffer);
			Uniforms[i].Name = std::string(buffer);
			Uniforms[i].Location = glGetUniformLocation(ID, buffer);
		}
	}

	void Shader::checkCompileErrors(unsigned int shader, std::string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
}
