



#include "CommandBuffer.h"

namespace Exp
{
	void CommandBuffer::Push(Mesh* mesh, Material* material, const glm::mat4& transform, bool castShadows)
	{
		RenderCommand command;
		command.mesh = mesh;
		command.material = material;
		command.transform = transform;
		command.castShadows = castShadows;

		if (material->type == Material::MaterialType::MATERIAL_DEFAULT)
		{
			deferredCommands.push_back(command);
		}
		else if (material->type == Material::MaterialType::MATERIAL_CUSTOM)
		{
			forwardCommands.push_back(command);
		}

	}

	void CommandBuffer::Clear()
	{
		deferredCommands.clear();
		forwardCommands.clear();
	}

	void CommandBuffer::Sort()
	{
		// Group draws by shader to limit program switches.
		// (A null shader sorts as 0 so the comparator stays a strict weak ordering.)
		auto shaderId = [](const RenderCommand& c) -> unsigned int
		{
			return c.material->GetShader() ? c.material->GetShader()->id : 0u;
		};
		std::sort(deferredCommands.begin(), deferredCommands.end(), [&](const RenderCommand& a, const RenderCommand& b)
			{
				return shaderId(a) > shaderId(b);
			});
	}

}

