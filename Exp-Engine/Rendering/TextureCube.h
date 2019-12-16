#pragma once

#include <vector>
#include "Texture.h"

namespace Exp
{
    class TextureCube
    {
    public:
        unsigned int id = -1;

        GLenum internalFormat = GL_RGBA;            // the format each texel is stored in
        GLenum format         = GL_RGBA;            // the format each texel is stored in
        GLenum type           = GL_UNSIGNED_BYTE;
        GLenum filterMin      = GL_LINEAR;          // what filter method to use during minification
        GLenum filterMax      = GL_LINEAR;          // what filter method to use during magnification
        GLenum wrapS          = GL_CLAMP_TO_EDGE;   // wrapping method of the S coordinate
        GLenum wrapT          = GL_CLAMP_TO_EDGE;   // wrapping method of the T coordinate
        GLenum wrapR          = GL_CLAMP_TO_EDGE;   // wrapping method of the T coordinate
        bool   mipmapping = false;

        unsigned int faceWidth  = 0;
        unsigned int faceHeight = 0;
    private:
    public:
        TextureCube();
        ~TextureCube();

        // default initialize all cubemap faces w/ default values
        void DefaultInitialize(unsigned int width, unsigned int height, GLenum format, GLenum type, bool mipmap = false);

        // cubemap texture generation per face
        void GenerateFace(GLenum face, unsigned int width, unsigned int height, GLenum format, GLenum type, unsigned char* data);
        //void Generate(unsigned int width, unsigned int height, GLenum dataFormat, GLenum type, std::vector<Texture> faces);
        void SetMipFace(GLenum face, unsigned int width, unsigned int height, GLenum format, GLenum type, unsigned int mipLevel, unsigned char* data);

        // resize (note that its values will be uninitialized)
        void Resize(unsigned int width, unsigned int height);

        void Bind(int unit = -1);
        void Unbind();
    };
}