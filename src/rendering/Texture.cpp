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
		glGenTextures(1, &id);

		this->width = width;
		this->height = height;
		this->internalFormat = internalFormat;
		this->format = format;
		this->type = type;

		assert(this->target == GL_TEXTURE_2D);
		Bind();
		glTexImage2D(this->target, 0, this->internalFormat, this->width, this->height, 0, this->format, this->type, data);
		glTexParameteri(this->target, GL_TEXTURE_MIN_FILTER, this->filterMin);
		glTexParameteri(this->target, GL_TEXTURE_MAG_FILTER, this->filterMax);
		glTexParameteri(this->target, GL_TEXTURE_WRAP_S, this->wrapS);
		glTexParameteri(this->target, GL_TEXTURE_WRAP_T, this->wrapT);
		if (this->mipmapping)
			glGenerateMipmap(this->target);
		Unbind();
	}

	void Texture::Resize(unsigned int width, unsigned int height, unsigned int depth)
	{
		Bind();
		if (this->target == GL_TEXTURE_1D)
		{
			this->width = width;
			glTexImage1D(GL_TEXTURE_1D, 0, this->internalFormat, width, 0, this->format, this->type, 0);
		}
		else if (this->target == GL_TEXTURE_2D)
		{
			assert(height > 0);
			this->width = width;
			this->height = height;
			glTexImage2D(GL_TEXTURE_2D, 0, this->internalFormat, width, height, 0, this->format, this->type, 0);
		}
		else if (this->target == GL_TEXTURE_3D)
		{
			assert(height > 0 && depth > 0);
			this->width = width;
			this->height = height;
			this->depth = depth;
			glTexImage3D(GL_TEXTURE_3D, 0, this->internalFormat, width, height, depth, 0, this->format, this->type, 0);
		}
	}

	void Texture::Bind(int textureUnit)
	{
		if (textureUnit >= 0)
		{
			glActiveTexture(GL_TEXTURE0 + textureUnit);
		}

		glBindTexture(this->target, this->id);
	}

	void Texture::Unbind()
	{
		glBindTexture(this->target, 0);
	}

}
