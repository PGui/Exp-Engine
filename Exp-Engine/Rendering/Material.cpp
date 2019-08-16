#include "Material.h"

int Exp::Material::CurrentMaterialId = 0;

namespace Exp
{
	Material::Material(Shader * shader) :
		MatShader(shader),
		MaterialId(++CurrentMaterialId)
	{
	}

	Material::~Material()
	{
	}

	Material & Material::operator=(const Material & material)
	{
		this->MatShader = material.MatShader;

		this->Type = material.Type;

		this->Uniforms = material.Uniforms;
		this->SamplerUniforms = material.SamplerUniforms;

		//this->Color = material.Color;

		this->DepthTest = material.DepthTest;
		this->DepthWrite = material.DepthWrite;
		this->DepthCompare = material.DepthCompare;

		this->Cull = material.Cull;
		this->CullFace = material.CullFace;
		this->CullWindingOrder = material.CullWindingOrder;

		this->Blend = material.Blend;
		this->BlendSrc = material.BlendSrc;
		this->BlendDst = material.BlendDst;
		this->BlendEquation = material.BlendEquation;

		return *this;
	}

	void Material::SetShader(Shader * shader)
	{
		MatShader = shader;
	}

	Shader * Material::GetShader()
	{
		return MatShader;
	}

	const std::map<std::string, UniformValueSampler> & Material::GetSamplerUniforms()
	{
		return SamplerUniforms;
	}

	const std::map<std::string, UniformValue> & Material::GetUniforms()
	{
		return Uniforms;
	}

	void Material::SetTexture(std::string name, Texture * texture, unsigned int unit)
	{
		SamplerUniforms[name].Unit = unit;
		SamplerUniforms[name].Texture = texture;
		switch (texture->Target)
		{
		case GL_TEXTURE_1D:
			SamplerUniforms[name].Type = SHADER_TYPE_SAMPLER1D;
			break;
		case GL_TEXTURE_2D:
			SamplerUniforms[name].Type = SHADER_TYPE_SAMPLER2D;
			break;
		case GL_TEXTURE_3D:
			SamplerUniforms[name].Type = SHADER_TYPE_SAMPLER3D;
			break;
		case GL_TEXTURE_CUBE_MAP:
			SamplerUniforms[name].Type = SHADER_TYPE_SAMPLERCUBE;
			break;
		}

		if (MatShader)
		{
			MatShader->use();
			MatShader->setInt(name, unit);
		}

	}

	void Material::SetInt(std::string name, int value)
	{
		Uniforms[name].Type = SHADER_TYPE_INT;
		Uniforms[name].Int = value;
	}

	void Material::SetFloat(std::string name, float value)
	{
		Uniforms[name].Type = SHADER_TYPE_FLOAT;
		Uniforms[name].Float = value;
	}

	void Material::SetVector(std::string name, glm::vec3 value)
	{
		Uniforms[name].Type = SHADER_TYPE_VEC3;
		Uniforms[name].Vec3 = value;
	}
}
