#pragma once
#include <glad/glad.h>

namespace Exp
{

	class GLCache
	{
	private:
		// gl toggles
		bool m_DepthTest;
		bool m_Blend;
		bool m_CullFace;

		// gl state
		GLenum m_DepthFunc;
		GLenum m_BlendSrc;
		GLenum m_BlendDst;
		GLenum m_FrontFace;
		GLenum m_PolygonMode;

		// shaders
		unsigned int m_ActiveShaderID;
	public:
		GLCache();
		~GLCache();

		// update GL state if requested state is different from current GL state.
		void SetDepthTest(bool enable);
		void SetDepthFunc(GLenum depthFunc);
		void SetBlend(bool enable);
		void SetBlendFunc(GLenum src, GLenum dst);
		void SetCull(bool enable);
		void SetCullFace(GLenum face);
		void SetPolygonMode(GLenum mode);

		void Reset(); // Reset cache for rendering.

		// switch shader only if a different ID is requested.
		// note that we won't use this too often, as we already sort render state on shader ID.
		void SwitchShader(unsigned int ID);
	};
}
