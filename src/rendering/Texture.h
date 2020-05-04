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
		unsigned int id;

		GLenum target = GL_TEXTURE_2D;           // what type of texture we're dealing with
		GLenum internalFormat = GL_RGBA;                 // number of color components
		GLenum format = GL_RGBA;                 // the format each texel is stored in
		GLenum type = GL_UNSIGNED_BYTE;
		GLenum filterMin = GL_LINEAR_MIPMAP_LINEAR; // what filter method to use during minification
		GLenum filterMax = GL_LINEAR;               // what filter method to use during magnification
		GLenum wrapS = GL_REPEAT;               // wrapping method of the S coordinate
		GLenum wrapT = GL_REPEAT;               // wrapping method of the T coordinate
		GLenum wrapR = GL_REPEAT;               // wrapping method of the R coordinate
		bool mipmapping = true;

		unsigned int width = 0;
		unsigned int height = 0;
		unsigned int depth = 0;

		void Generate(unsigned int width, unsigned int height, GLenum internalFormat, GLenum format, GLenum type, void* data);

		void Resize(unsigned int width, unsigned int height = 0, unsigned int depth = 0);

		void Bind(int textureUnit = -1);
		void Unbind();
	};
}
