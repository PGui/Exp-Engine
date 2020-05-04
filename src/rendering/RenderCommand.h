#pragma once
#include <glm/glm.hpp>

namespace Exp
{
	class Mesh;
	class Material;

	struct RenderCommand
	{
		glm::mat4 transform;
		Mesh* mesh;
		Material* material;
	};
}
