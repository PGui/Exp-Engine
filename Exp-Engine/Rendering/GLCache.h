#pragma once
#include <glad/glad.h>

namespace Exp
{

	class GLCache
	{
	private:
		// gl toggles
		bool depthTest	= true;
		bool blend		= false;
		bool cullFace		= true;

		// gl state
		GLenum depthFunc	 = GL_LESS;
		GLenum blendSrc	 = GL_ONE;
		GLenum blendDst    = GL_ONE_MINUS_SRC_ALPHA;
		GLenum frontFace	 = GL_BACK;
		GLenum polygonMode = GL_FILL;

		// shaders
		unsigned int activeShaderID = -1;
	public:
		static GLCache& GetInstance()
		{
			static GLCache instance; 
			return instance;
		}
		

		// update GL state if requested state is different from current GL state.
		void SetDepthTest(bool enable, bool force = false);
		void SetDepthFunc(GLenum value, bool force = false);
		void SetBlend(bool enable, bool force = false);
		void SetBlendFunc(GLenum src, GLenum dst, bool force = false);
		void SetCull(bool enable, bool force = false);
		void SetCullFace(GLenum face, bool force = false);
		void SetPolygonMode(GLenum mode, bool force = false);

		void Reset(bool force = false); // Reset cache for rendering.

		// switch shader only if a different id is requested.
		// note that we won't use this too often, as we already sort render state on shader id.
		void SwitchShader(unsigned int id);
	private:
		GLCache() {}
	public:
		GLCache(GLCache const&) = delete;
		void operator=(GLCache const&) = delete;
	};
}
