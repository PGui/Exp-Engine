#pragma once
#include "Light.h"

namespace Exp
{
	class PointLight :
		public Light
	{
	public:
		PointLight();
		virtual ~PointLight();

		glm::vec3 position = glm::vec3(0.0f);
		float radius = 1.0f;
	};

}