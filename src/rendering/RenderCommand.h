#pragma once
#include <glm/glm.hpp>

namespace Exp
{
	class Mesh;
	class Material;

	struct RenderCommand
	{
		glm::mat4 transform = glm::mat4(1.0f);
		Mesh* mesh = nullptr;
		Material* material = nullptr;
		bool castShadows = true;
	};
}
