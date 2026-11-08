#pragma once
#include <vector>
#include <algorithm>

#include <glm/glm.hpp>

#include "../mesh/Mesh.h"

#include "RenderCommand.h"
#include "Material.h"
#include "RenderCommand.h"




namespace Exp
{
	class Mesh;
	class Material;

	class CommandBuffer
	{

	public:
		std::vector<RenderCommand> deferredCommands;
		std::vector<RenderCommand> forwardCommands;


	public:

		void Push(Mesh* mesh, Material* material, const glm::mat4& transform, bool castShadows = true);

		void Clear();

		void Sort();

	};
}
