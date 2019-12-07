#pragma once
#include <vector>
#include <algorithm>

#include "../Glm/glm.hpp"

#include "../Mesh/Mesh.h"

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
		std::vector<RenderCommand> DeferredCommands;
		std::vector<RenderCommand> ForwardCommands;


	public:

		void Push(Mesh* mesh, Material* Material, glm::mat4 transform);

		void Clear();

		void Sort();

	};
}
