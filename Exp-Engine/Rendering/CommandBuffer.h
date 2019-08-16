#pragma once
#include "../Glm/glm.hpp"
#include "RenderCommand.h"
#include <vector>


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
