#pragma once

#include <vector>
#include <iostream>
#include "Texture.h"

namespace Exp
{
	class RenderTarget
	{
	public:
		unsigned int ID;

		unsigned int Width;
		unsigned int Height;

		GLenum Type = GL_UNSIGNED_BYTE;

		bool bHasDepthAndStencil = true;
	private:
		GLenum					Target = GL_TEXTURE_2D;
		Texture					DepthStencil;
		std::vector<Texture>	ColorAttachments;
	public:
		RenderTarget(unsigned int width, unsigned int height, GLenum type = GL_UNSIGNED_BYTE, unsigned int nrColorAttachments = 1, bool depthAndStencil = true);
		~RenderTarget();

		Texture* GetDepthStencilTexture();
		Texture* GetColorTexture(unsigned int index);

		void Resize(unsigned int width, unsigned int height);
		void SetTarget(GLenum target);
	};
}
