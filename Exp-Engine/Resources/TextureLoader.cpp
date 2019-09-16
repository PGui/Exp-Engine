#include "TextureLoader.h"
#include "../StbImage/stb_image.h"
#include "../Rendering/Texture.h"
#include "../Rendering/TextureCube.h"

namespace Exp
{
	TextureLoader::TextureLoader()
	{
	}

	TextureLoader::~TextureLoader()
	{
	}

	Texture TextureLoader::loadTexture(std::string path, GLenum target, GLenum internalFormat, bool srgb)
	{
		assert(target == GL_TEXTURE_2D);
		Texture texture;
		stbi_set_flip_vertically_on_load(true);

		texture.Target = target;
		texture.InternalFormat = internalFormat;
		if (texture.InternalFormat == GL_RGB || texture.InternalFormat == GL_SRGB)
			texture.InternalFormat = srgb ? GL_SRGB : GL_RGB;
		if (texture.InternalFormat == GL_RGBA || texture.InternalFormat == GL_SRGB_ALPHA)
			texture.InternalFormat = srgb ? GL_SRGB_ALPHA : GL_RGBA;



		int width, height, nrComponents;
		unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			if (texture.Target == GL_TEXTURE_2D)
				texture.Generate(width, height, texture.InternalFormat, format, GL_UNSIGNED_BYTE, (void*)data);

			//TODO Other types of textures

			stbi_image_free(data);
		}
		else
		{
			spdlog::error("Texture failed to load at path {}", path);
			stbi_image_free(data);
		}

		return texture;
	}

	TextureCube TextureLoader::LoadTextureCube(std::string top, std::string bottom, std::string left, std::string right, std::string front, std::string back)
	{
		TextureCube texture;

		// disable y flip on cubemaps
		stbi_set_flip_vertically_on_load(false);

		std::vector<std::string> faces = { top, bottom, left, right, front, back };
		for (unsigned int i = 0; i < faces.size(); ++i)
		{
			int width, height, nrComponents;
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);

			if (data)
			{
				GLenum format;
				if (nrComponents == 3)
					format = GL_RGB;
				else
					format = GL_RGBA;

				texture.GenerateFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, width, height, format, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				spdlog::error("Cube texture at path {} failed to load", faces[i]);
				stbi_image_free(data);
				return texture;
			}
		}
		if (texture.Mipmapping)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		return texture;
	}
	// --------------------------------------------------------------------------------------------
	TextureCube TextureLoader::LoadTextureCube(std::string folder)
	{
		return TextureLoader::LoadTextureCube(folder + "right.jpg",
			folder + "left.jpg",
			folder + "top.jpg",
			folder + "bottom.jpg",
			folder + "front.jpg",
			folder + "back.jpg");
	}
}