#pragma once
#include <glad/glad.h>

namespace Exp
{

	class GLCache
	{
	private:
		// gl toggles
		bool m_DepthTest	= true;
		bool m_Blend		= false;
		bool m_CullFace		= true;

		// gl state
		GLenum m_DepthFunc	 = GL_LESS;
		GLenum m_BlendSrc	 = GL_ONE;
		GLenum m_BlendDst    = GL_ONE_MINUS_SRC_ALPHA;
		GLenum m_FrontFace	 = GL_BACK;
		GLenum m_PolygonMode = GL_FILL;

		// shaders
		unsigned int m_ActiveShaderID = -1;
	public:
		static GLCache& getInstance()
		{
			static GLCache instance; 
			return instance;
		}
		

		// update GL state if requested state is different from current GL state.
		void SetDepthTest(bool enable, bool force = false);
		void SetDepthFunc(GLenum depthFunc, bool force = false);
		void SetBlend(bool enable, bool force = false);
		void SetBlendFunc(GLenum src, GLenum dst, bool force = false);
		void SetCull(bool enable, bool force = false);
		void SetCullFace(GLenum face, bool force = false);
		void SetPolygonMode(GLenum mode, bool force = false);

		void Reset(bool force = false); // Reset cache for rendering.

		// switch shader only if a different ID is requested.
		// note that we won't use this too often, as we already sort render state on shader ID.
		void SwitchShader(unsigned int ID);
	private:
		GLCache() {}
	public:
		GLCache(GLCache const&) = delete;
		void operator=(GLCache const&) = delete;
	};
}
