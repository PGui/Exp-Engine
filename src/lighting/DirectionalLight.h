#pragma once
#include <memory>
#include "Light.h"
#include "../rendering/ShadowMap.h"

namespace Exp
{
	class DirectionalLight:
		public Light
	{
	public:
		DirectionalLight();
		virtual ~DirectionalLight();

		// Direction from the scene towards the light.
		glm::vec3 direction = glm::vec3(1,0,0);

		// Created lazily by the renderer when castShadows is enabled.
		std::unique_ptr<ShadowMap> shadowMap;
	};

}


