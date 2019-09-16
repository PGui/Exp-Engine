#include "RenderTarget.h"

namespace Exp
{
	RenderTarget::RenderTarget(unsigned int width, unsigned int height, GLenum type, unsigned int nrColorAttachments, bool depthAndStencil)
	{
		Width = width;
		Height = height;
		Type = type;

		glGenFramebuffers(1, &ID);
		glBindFramebuffer(GL_FRAMEBUFFER, ID);

		for (unsigned int i = 0; i < nrColorAttachments; ++i)
		{
			Texture texture;
			texture.FilterMin = GL_LINEAR;
			texture.FilterMax = GL_LINEAR;
			texture.WrapS = GL_CLAMP_TO_EDGE;
			texture.WrapT = GL_CLAMP_TO_EDGE;
			texture.Mipmapping = false;

			GLenum internalFormat = GL_RGBA;
			if (type == GL_HALF_FLOAT)
				internalFormat = GL_RGBA16F;
			else if (type == GL_FLOAT)
				internalFormat = GL_RGBA32F;
			texture.Generate(width, height, internalFormat, GL_RGBA, type, 0);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture.ID, 0);

			GLuint depthrenderbuffer;
			glGenRenderbuffers(1, &depthrenderbuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);


			ColorAttachments.push_back(texture);
		}

		bHasDepthAndStencil = depthAndStencil;
		if (bHasDepthAndStencil)
		{
			Texture texture;
			texture.FilterMin = GL_LINEAR;
			texture.FilterMax = GL_LINEAR;
			texture.WrapS = GL_CLAMP_TO_EDGE;
			texture.WrapT = GL_CLAMP_TO_EDGE;
			texture.Mipmapping = false;
			texture.Generate(width, height, GL_DEPTH_STENCIL, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture.ID, 0);
			DepthStencil = texture;
		}
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			spdlog::error("Framebuffer is not complete !");
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	RenderTarget::~RenderTarget()
	{
		glDeleteFramebuffers(1, &ID);
	}

	Texture* RenderTarget::GetDepthStencilTexture()
	{
		return &DepthStencil;
	}

	Texture* RenderTarget::GetColorTexture(unsigned int index)
	{
		if (index < ColorAttachments.size())
			return &ColorAttachments[index];
		else
		{
			spdlog::error("RenderTarget color texture requested, but not available : {}", index);
			return nullptr;
		}
	}

	void RenderTarget::Resize(unsigned int width, unsigned int height)
	{
		Width = width;
		Height = height;

		for (unsigned int i = 0; i < ColorAttachments.size(); ++i)
		{
			ColorAttachments[i].Resize(width, height);
		}
		// generate Depth/Stencil texture if requested
		if (bHasDepthAndStencil)
		{
			DepthStencil.Resize(width, height);
		}
	}

	void RenderTarget::SetTarget(GLenum target)
	{
		Target = target;
	}
}