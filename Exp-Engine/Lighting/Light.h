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

		glm::vec3 color = glm::vec3(1.0f, 1.0f ,1.0f);
		float intensity = 1.0f;

		bool			castShadows = true;
		RenderTarget*	shadowMapRT = nullptr;
		glm::mat4		lightSpaceViewProjection = glm::mat4(1.0f);


		//Debug
		bool       visible = true;
		bool       renderMesh = false;
	};
}

