#pragma once
#include <string>
#include <iostream>
#include <assert.h>
#include <spdlog/spdlog.h>

#include "stb_image.h"
#include "../Rendering/Texture.h"
#include "../Rendering/TextureCube.h"

namespace Exp
{
	class Texture;
	class TextureCube;

	class TextureLoader
	{
	public:
		TextureLoader();
		~TextureLoader();

		static Texture loadTexture(std::string path, GLenum target, GLenum internalFormat, bool srgb);
		static TextureCube LoadTextureCube(std::string top, std::string bottom, std::string left, std::string right, std::string front, std::string back);
		static TextureCube LoadTextureCube(std::string folder);
	};
}

