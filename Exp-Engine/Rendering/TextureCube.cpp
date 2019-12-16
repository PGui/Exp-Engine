#include "TextureCube.h"

namespace Exp
{
    // --------------------------------------------------------------------------------------------
    TextureCube::TextureCube()
    {

    }
    // --------------------------------------------------------------------------------------------
    TextureCube::~TextureCube()
    {

    }
    // --------------------------------------------------------------------------------------------
    void TextureCube::DefaultInitialize(unsigned int width, unsigned int height, GLenum format, GLenum type, bool mipmap)
    {
        glGenTextures(1, &id);

        this->faceWidth = width;
        this->faceHeight = height;
        this->format = format;
        this->type = type;
        this->mipmapping = mipmap;

        if (type == GL_HALF_FLOAT && format == GL_RGB)
            this->internalFormat = GL_RGB16F;
        else if (type == GL_FLOAT && format == GL_RGB)
            this->internalFormat = GL_RGB32F;
        else if (type == GL_HALF_FLOAT && format == GL_RGBA)
            this->internalFormat = GL_RGBA16F;
        else if (type == GL_FLOAT && format == GL_RGBA)
            this->internalFormat = GL_RGBA32F;

        Bind();
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, this->filterMin);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, this->filterMax);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, this->wrapS);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, this->wrapT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, this->wrapR);

        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, this->internalFormat, width, height, 0, format, type, NULL);
        }
        if (mipmap)
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
    // --------------------------------------------------------------------------------------------
    void TextureCube::GenerateFace(GLenum face, unsigned int width, unsigned int height, GLenum format, GLenum type, unsigned char* data)
    {
        if(this->faceWidth == 0)
            glGenTextures(1, &id);

        this->faceWidth = width;
        this->faceHeight = height;
        this->format = format;
        this->type = type;

        Bind();

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, this->filterMin);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, this->filterMax);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, this->wrapS);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, this->wrapT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, this->wrapR);

        glTexImage2D(face, 0, format, width, height, 0, format, type, data);
    }
    // --------------------------------------------------------------------------------------------
    void TextureCube::SetMipFace(GLenum face, unsigned int width, unsigned int height, GLenum format, GLenum type, unsigned int mipLevel, unsigned char *data)
    {
        Bind();
        glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mipLevel, 0, 0, width, height, format, type, data);
    }
    // --------------------------------------------------------------------------------------------
    void TextureCube::Resize(unsigned int width, unsigned int height)
    {
        this->faceWidth = width;
        this->faceHeight = height;

        Bind();
        for (unsigned int i = 0; i < 6; ++i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, this->format, width, height, 0, this->format, this->type, NULL);
    }
    // --------------------------------------------------------------------------------------------
    void TextureCube::Bind(int unit)
    {
        if (unit >= 0)
            glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    }
    // --------------------------------------------------------------------------------------------
    void TextureCube::Unbind()
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
}