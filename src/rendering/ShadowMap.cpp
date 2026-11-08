#include "ShadowMap.h"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

namespace Exp
{
	ShadowMap::ShadowMap(unsigned int resolution) :
		resolution(resolution)
	{
		Create();
	}

	ShadowMap::~ShadowMap()
	{
		Destroy();
	}

	void ShadowMap::Resize(unsigned int newResolution)
	{
		if (newResolution == resolution)
			return;
		resolution = newResolution;
		Destroy();
		Create();
	}

	void ShadowMap::Create()
	{
		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Outside of the shadow map = lit.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		const float border[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			spdlog::error("Shadow map framebuffer is not complete");
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void ShadowMap::Destroy()
	{
		if (fbo)
			glDeleteFramebuffers(1, &fbo);
		if (depthTexture)
			glDeleteTextures(1, &depthTexture);
		fbo = 0;
		depthTexture = 0;
	}

	void ShadowMap::BeginRender()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, resolution, resolution);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void ShadowMap::BindTexture(unsigned int unit) const
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
	}

	glm::mat4 ShadowMap::ComputeDirectionalViewProjection(const glm::vec3& lightDirection, const glm::vec3& center, float radius, unsigned int resolution)
	{
		// lightDirection points from the scene towards the light.
		const glm::vec3 dir = glm::normalize(lightDirection);
		const glm::vec3 up = std::abs(dir.y) > 0.99f ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);

		glm::mat4 view = glm::lookAt(center + dir * radius * 2.0f, center, up);
		const glm::mat4 projection = glm::ortho(-radius, radius, -radius, radius, 0.01f, radius * 4.0f);

		// Texel snapping: round the projected origin to a whole texel.
		glm::mat4 viewProjection = projection * view;
		glm::vec4 origin = viewProjection * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		const float halfRes = resolution * 0.5f;
		const glm::vec2 originTexels = glm::vec2(origin) * halfRes;
		const glm::vec2 offset = (glm::round(originTexels) - originTexels) / halfRes;
		glm::mat4 snap(1.0f);
		snap[3][0] = offset.x;
		snap[3][1] = offset.y;
		return snap * viewProjection;
	}
}
