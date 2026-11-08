#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Exp
{
	/*
		Depth-only render target used for shadow mapping.
		The depth texture has hardware comparison enabled, so it is sampled
		in shaders with a sampler2DShadow (free 2x2 PCF with linear filtering).
	*/
	class ShadowMap
	{
	public:
		explicit ShadowMap(unsigned int resolution = 2048);
		~ShadowMap();

		ShadowMap(const ShadowMap&) = delete;
		ShadowMap& operator=(const ShadowMap&) = delete;

		void Resize(unsigned int resolution);

		// Binds the framebuffer, sets the viewport and clears depth.
		void BeginRender();

		void BindTexture(unsigned int unit) const;

		unsigned int GetResolution() const { return resolution; }

		// Orthographic light-space matrix covering a sphere of 'radius' around 'center',
		// snapped to shadow-map texels to avoid shimmering when the camera moves.
		static glm::mat4 ComputeDirectionalViewProjection(const glm::vec3& lightDirection, const glm::vec3& center, float radius, unsigned int resolution);

	private:
		GLuint fbo = 0;
		GLuint depthTexture = 0;
		unsigned int resolution = 0;

		void Create();
		void Destroy();
	};
}
