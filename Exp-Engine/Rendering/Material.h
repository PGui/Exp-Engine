#pragma once
#include <map>

#include "Shader.h"
#include "Texture.h"
#include "TextureCube.h"

#include "ShadingTypes.h"

namespace Exp
{
	class Material
	{
		static int currentMaterialId;
	public:
		enum class MaterialType
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

		void SetTextureCube(std::string name, TextureCube* value, unsigned int unit);

		void SetInt(std::string name, int value);
		void SetFloat(std::string name, float value);
		void SetVector(std::string name, glm::vec3 value);

		int materialId;

		MaterialType type = MaterialType::MATERIAL_DEFAULT;

		// depth state
		bool   depthTest = true;
		bool   depthWrite = true;
		GLenum depthCompare = GL_LESS;

		// face culling state
		bool   cull = true;
		GLenum cullFace = GL_BACK;
		GLenum cullWindingOrder = GL_CCW;

		// blending state
		bool   blend = false;
		GLenum blendSrc = GL_ONE; // pre-multiplied alpha
		GLenum blendDst = GL_ONE_MINUS_SRC_ALPHA;
		GLenum blendEquation = GL_FUNC_ADD;

		// shadow state
		bool shadowCast = true;
		bool shadowReceive = true;
	private:
		std::map<std::string, UniformValueSampler> samplerUniforms;
		std::map<std::string, UniformValue>        uniforms;

		Shader * shader;
	};
}
