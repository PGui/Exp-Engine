#include "Material.h"

int Exp::Material::currentMaterialId = 0;

namespace Exp
{
	Material::Material(Shader * shader) :
		shader(shader),
		materialId(++currentMaterialId)
	{
	}

	Material::~Material()
	{
	}

	Material & Material::operator=(const Material & material)
	{
		this->shader = material.shader;

		this->type = material.type;

		this->uniforms = material.uniforms;
		this->samplerUniforms = material.samplerUniforms;

		//this->Color = material.Color;

		this->depthTest = material.depthTest;
		this->depthWrite = material.depthWrite;
		this->depthCompare = material.depthCompare;

		this->cull = material.cull;
		this->cullFace = material.cullFace;
		this->cullWindingOrder = material.cullWindingOrder;

		this->blend = material.blend;
		this->blendSrc = material.blendSrc;
		this->blendDst = material.blendDst;
		this->blendEquation = material.blendEquation;

		return *this;
	}

	void Material::SetShader(Shader * shader)
	{
		shader = shader;
	}

	Shader * Material::GetShader()
	{
		return shader;
	}

	const std::map<std::string, UniformValueSampler> & Material::GetSamplerUniforms()
	{
		return samplerUniforms;
	}

	const std::map<std::string, UniformValue> & Material::GetUniforms()
	{
		return uniforms;
	}

	void Material::SetTexture(std::string name, Texture * texture, unsigned int unit)
	{
		samplerUniforms[name].Unit = unit;
		samplerUniforms[name].Texture = texture;
		switch (texture->Target)
		{
		case GL_TEXTURE_1D:
			samplerUniforms[name].type = SHADER_TYPE_SAMPLER1D;
			break;
		case GL_TEXTURE_2D:
			samplerUniforms[name].type = SHADER_TYPE_SAMPLER2D;
			break;
		case GL_TEXTURE_3D:
			samplerUniforms[name].type = SHADER_TYPE_SAMPLER3D;
			break;
		case GL_TEXTURE_CUBE_MAP:
			samplerUniforms[name].type = SHADER_TYPE_SAMPLERCUBE;
			break;
		}

		if (shader)
		{
			shader->Use();
			shader->SetInt(name, unit);
		}

	}

	void Material::SetTextureCube(std::string name, TextureCube* value, unsigned int unit)
	{
		samplerUniforms[name].Unit = unit;
		samplerUniforms[name].type = SHADER_TYPE_SAMPLERCUBE;
		samplerUniforms[name].TextureCube = value;

		if (shader)
		{
			shader->Use();
			shader->SetInt(name, unit);
		}
	}

	void Material::SetInt(std::string name, int value)
	{
		uniforms[name].type = SHADER_TYPE_INT;
		uniforms[name].Int = value;
	}

	void Material::SetFloat(std::string name, float value)
	{
		uniforms[name].type = SHADER_TYPE_FLOAT;
		uniforms[name].Float = value;
	}

	void Material::SetVector(std::string name, glm::vec3 value)
	{
		uniforms[name].type = SHADER_TYPE_VEC3;
		uniforms[name].Vec3 = value;
	}
}
