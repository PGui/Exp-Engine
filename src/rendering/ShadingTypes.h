#pragma once
#include <glm/glm.hpp>

namespace Exp
{
	class Texture;
	class TextureCube;

	enum SHADER_TYPE
	{
		SHADER_TYPE_BOOL,
		SHADER_TYPE_INT,
		SHADER_TYPE_FLOAT,
		SHADER_TYPE_SAMPLER1D,
		SHADER_TYPE_SAMPLER2D,
		SHADER_TYPE_SAMPLER3D,
		SHADER_TYPE_SAMPLERCUBE,
		SHADER_TYPE_VEC2,
		SHADER_TYPE_VEC3,
		SHADER_TYPE_VEC4,
		SHADER_TYPE_MAT2,
		SHADER_TYPE_MAT3,
		SHADER_TYPE_MAT4,
	};

	struct UniformValueSampler
	{
		SHADER_TYPE  type;
		unsigned int Unit;
		union
		{
			Texture     *Texture;
			TextureCube* TextureCube;
		};

		UniformValueSampler() {}
	};

	struct UniformValue
	{
		SHADER_TYPE type;
		// TODO(Joey): now each element takes up the space of its largest 
		// element (mat4) which is 64 bytes; come up with a better solution!
		union
		{
			bool       Bool;
			int        Int;
			float      Float;

			glm::vec2 Vec2;
			glm::vec3 Vec3;
			glm::vec4 Vec4;
			glm::mat2 Mat2;
			glm::mat3 Mat3;
			glm::mat4 Mat4;
		};

		UniformValue() {}
	};

	struct Uniform
	{
		std::string  name;
		int          Size;
		unsigned int Location;
	};

	struct VertexAttribute
	{
		std::string  name;
		int          Size;
		unsigned int Location;
	};
}
