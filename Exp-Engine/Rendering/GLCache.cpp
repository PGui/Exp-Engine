#include "GLCache.h"

namespace Exp
{
	// --------------------------------------------------------------------------------------------
	void GLCache::SetDepthTest(bool enable, bool force)
	{
		if (m_DepthTest != enable || force)
		{
			m_DepthTest = enable;
			if (enable)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}

	}
	// --------------------------------------------------------------------------------------------
	void GLCache::SetDepthFunc(GLenum depthFunc, bool force)
	{
		if (m_DepthFunc != depthFunc || force)
		{
			m_DepthFunc = depthFunc;
			glDepthFunc(depthFunc);
		}
	}
	// --------------------------------------------------------------------------------------------
	void GLCache::SetBlend(bool enable, bool force)
	{
		if (m_Blend != enable || force)
		{
			m_Blend = enable;
			if (enable)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);
		}
	}
	// --------------------------------------------------------------------------------------------
	void GLCache::SetBlendFunc(GLenum src, GLenum dst, bool force)
	{
		if (m_BlendSrc != src || m_BlendDst != dst || force)
		{
			m_BlendSrc = src;
			m_BlendDst = dst;
			glBlendFunc(src, dst);
		}
	}
	// --------------------------------------------------------------------------------------------
	void GLCache::SetCull(bool enable, bool force)
	{
		if (m_CullFace != enable || force)
		{
			m_CullFace = enable;
			if (enable)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}
	}
	// --------------------------------------------------------------------------------------------
	void GLCache::SetCullFace(GLenum face, bool force)
	{
		if (m_FrontFace != face || force)
		{
			m_FrontFace = face;
			glCullFace(face);
		}
	}
	// --------------------------------------------------------------------------------------------
	void GLCache::SetPolygonMode(GLenum mode, bool force)
	{
		if (m_PolygonMode != mode || force)
		{
			m_PolygonMode = mode;
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
	void GLCache::SwitchShader(unsigned int ID)
	{
		if (m_ActiveShaderID != ID)
		{
			glUseProgram(ID);
		}
	}
}
