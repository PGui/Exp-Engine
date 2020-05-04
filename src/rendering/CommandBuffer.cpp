



#include "CommandBuffer.h"

namespace Exp
{
	void CommandBuffer::Push(Mesh* mesh, Material* material, glm::mat4 transform)
	{
		RenderCommand command;
		command.mesh = mesh;
		command.material = material;
		command.transform = transform;

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
		std::sort(deferredCommands.begin(), deferredCommands.end(), [](const RenderCommand& a, const RenderCommand& b) -> bool
			{
				if (!a.material->GetShader() || !b.material->GetShader())
					return false;
				return a.material->GetShader()->id > b.material->GetShader()->id;
			});
	}

}

