#include "Texture.h"


namespace Exp
{

	Texture::Texture()
	{
	}


	Texture::~Texture()
	{
	}

	void Texture::Generate(unsigned int width, unsigned int height, GLenum internalFormat, GLenum format, GLenum type, void* data)
	{
		glGenTextures(1, &ID);

		Width = width;
		Height = height;
		InternalFormat = internalFormat;
		Format = format;
		Type = type;

		assert(Target == GL_TEXTURE_2D);
		Bind();
		glTexImage2D(Target, 0, internalFormat, width, height, 0, format, type, data);
		glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, FilterMin);
		glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, FilterMax);
		glTexParameteri(Target, GL_TEXTURE_WRAP_S, WrapS);
		glTexParameteri(Target, GL_TEXTURE_WRAP_T, WrapT);
		if (Mipmapping)
			glGenerateMipmap(Target);
		Unbind();
	}

	void Texture::Resize(unsigned int width, unsigned int height, unsigned int depth)
	{
		Bind();
		if (Target == GL_TEXTURE_1D)
		{
			glTexImage1D(GL_TEXTURE_1D, 0, InternalFormat, width, 0, Format, Type, 0);
		}
		else if (Target == GL_TEXTURE_2D)
		{
			assert(height > 0);
			glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, width, height, 0, Format, Type, 0);
		}
		else if (Target == GL_TEXTURE_3D)
		{
			assert(height > 0 && depth > 0);
			glTexImage3D(GL_TEXTURE_3D, 0, InternalFormat, width, height, depth, 0, Format, Type, 0);
		}
	}

	void Texture::Bind(int textureUnit)
	{
		if (textureUnit >= 0)
		{
			glActiveTexture(GL_TEXTURE0 + textureUnit);
		}

		glBindTexture(Target, ID);
	}

	void Texture::Unbind()
	{
		glBindTexture(Target, 0);
	}

}
