#pragma once
#include "../glm/glm.hpp"

namespace Exp
{
	class Mesh;
	class Material;

	struct RenderCommand
	{
		glm::mat4 Transform;
		Mesh* Mesh;
		Material* Material;
	};
}
