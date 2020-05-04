#pragma once
#include "Light.h"

namespace Exp
{
	class DirectionalLight:
		public Light
	{
	public:
		DirectionalLight();
		virtual ~DirectionalLight();

		glm::vec3 direction = glm::vec3(1,0,0);
	};

}


