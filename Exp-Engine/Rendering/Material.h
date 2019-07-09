#pragma once
#include <map>

#include "Texture.h"
#include "Shader.h"
#include "ShadingTypes.h"

namespace Exp
{
	class Material
	{
	public:
		enum MaterialType
		{
			MATERIAL_DEFAULT,
			MATERIAL_CUSTOM,
			MATERIAL_POST_PROCESS,
		};

		explicit Material(Shader* shader);
		~Material();

		Material & operator = (const Material & material);

		void SetShader(Shader * shader);
		Shader * GetShader();

		const std::map<std::string, UniformValueSampler> & GetSamplerUniforms();
		const std::map<std::string, UniformValue> & GetUniforms();

		void SetTexture(std::string name, Texture* value, unsigned int unit = 0);

		void SetInt(std::string name, int value);
		void SetFloat(std::string name, float value);
		void SetVector(std::string name, glm::vec3 value);

		MaterialType Type = MaterialType::MATERIAL_DEFAULT;

		// depth state
		bool   DepthTest = true;
		bool   DepthWrite = true;
		GLenum DepthCompare = GL_LESS;

		// face culling state
		bool   Cull = true;
		GLenum CullFace = GL_BACK;
		GLenum CullWindingOrder = GL_CCW;

		// blending state
		bool   Blend = false;
		GLenum BlendSrc = GL_ONE; // pre-multiplied alpha
		GLenum BlendDst = GL_ONE_MINUS_SRC_ALPHA;
		GLenum BlendEquation = GL_FUNC_ADD;

		// shadow state
		bool ShadowCast = true;
		bool ShadowReceive = true;
	private:
		std::map<std::string, UniformValueSampler> SamplerUniforms;
		std::map<std::string, UniformValue>        Uniforms;

		Shader * MatShader;



	};


}
