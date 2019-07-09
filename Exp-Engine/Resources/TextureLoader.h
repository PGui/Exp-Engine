#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <assert.h>


namespace Exp
{
	class Texture;

	class TextureLoader
	{
	public:
		TextureLoader();
		~TextureLoader();

		static Texture loadTexture(std::string path, GLenum target, GLenum internalFormat, bool srgb);
	};
}

