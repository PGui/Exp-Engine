#include "GLCache.h"

namespace Exp
{
	// --------------------------------------------------------------------------------------------
	void GLCache::SetDepthTest(bool enable, bool force)
	{
		if (depthTest != enable || force)
		{
			depthTest = enable;
			if (enable)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}

	}
	// --------------------------------------------------------------------------------------------
	void GLCache::SetDepthFunc(GLenum value, bool force)
	{
		if (depthFunc != value || force)
		{
			depthFunc = value;
			glDepthFunc(value);
		}
	}
	// --------------------------------------------------------------------------------------------
	void GLCache::SetBlend(bool enable, bool force)
	{
		if (blend != enable || force)
		{
			blend = enable;
			if (enable)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);
		}
	}
	// --------------------------------------------------------------------------------------------
	void GLCache::SetBlendFunc(GLenum src, GLenum dst, bool force)
	{
		if (blendSrc != src || blendDst != dst || force)
		{
			blendSrc = src;
			blendDst = dst;
			glBlendFunc(src, dst);
		}
	}
	// --------------------------------------------------------------------------------------------
	void GLCache::SetCull(bool enable, bool force)
	{
		if (cullFace != enable || force)
		{
			cullFace = enable;
			if (enable)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}
	}
	// --------------------------------------------------------------------------------------------
	void GLCache::SetCullFace(GLenum face, bool force)
	{
		if (frontFace != face || force)
		{
			frontFace = face;
			glCullFace(face);
		}
	}
	// --------------------------------------------------------------------------------------------
	void GLCache::SetPolygonMode(GLenum mode, bool force)
	{
		if (polygonMode != mode || force)
		{
			polygonMode = mode;
			glPolygonMode(GL_FRONT_AND_BACK, mode);
		}
	}
	void GLCache::Reset(bool force)
	{
		SetBlend(false, force);
		SetCull(true, force);
		SetCullFace(GL_BACK, force);
		SetDepthTest(true, force);
		SetDepthFunc(GL_LESS, force);
	}
	// --------------------------------------------------------------------------------------------
	void GLCache::SwitchShader(unsigned int id)
	{
		if (activeShaderID != id)
		{
			glUseProgram(id);
		}
	}
}
