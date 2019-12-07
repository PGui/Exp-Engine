#pragma once
#include <glad/glad.h>
#include <iostream>
#include <assert.h>
#include <GLFW/glfw3.h>

namespace Exp
{
	class Texture
	{
	public:
		Texture();
		~Texture();

	public:
		unsigned int ID;

		GLenum Target = GL_TEXTURE_2D;           // what type of texture we're dealing with
		GLenum InternalFormat = GL_RGBA;                 // number of color components
		GLenum Format = GL_RGBA;                 // the format each texel is stored in
		GLenum Type = GL_UNSIGNED_BYTE;
		GLenum FilterMin = GL_LINEAR_MIPMAP_LINEAR; // what filter method to use during minification
		GLenum FilterMax = GL_LINEAR;               // what filter method to use during magnification
		GLenum WrapS = GL_REPEAT;               // wrapping method of the S coordinate
		GLenum WrapT = GL_REPEAT;               // wrapping method of the T coordinate
		GLenum WrapR = GL_REPEAT;               // wrapping method of the R coordinate
		bool Mipmapping = true;

		unsigned int Width = 0;
		unsigned int Height = 0;
		unsigned int Depth = 0;

		void Generate(unsigned int width, unsigned int height, GLenum internalFormat, GLenum format, GLenum type, void* data);

		void Resize(unsigned int width, unsigned int height = 0, unsigned int depth = 0);

		void Bind(int textureUnit = -1);
		void Unbind();
	};
}
