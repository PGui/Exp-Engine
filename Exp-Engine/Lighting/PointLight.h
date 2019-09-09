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

		glm::vec3 m_Position = glm::vec3(0.0f);
		float m_Radius = 1.0f;
	};

}


