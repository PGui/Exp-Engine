#pragma once
#include "..//Glm/glm.hpp"
#include "../Rendering/RenderTarget.h"

namespace Exp
{
	class Light
	{
	public:
		Light();
		virtual ~Light();

		glm::vec3 m_Color = glm::vec3(1.0f, 0.0f ,0.0f);
		float m_Intensity = 1.0f;

		bool			m_CastShadows = true;
		RenderTarget*	m_ShadowMapRT = nullptr;
		glm::mat4		m_LightSpaceViewProjection = glm::mat4(1.0f);


		//Debug
		bool       m_Visible = true;
		bool       m_RenderMesh = false;
	};
}

