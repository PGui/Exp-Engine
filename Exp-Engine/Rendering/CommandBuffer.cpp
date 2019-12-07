



#include "CommandBuffer.h"

namespace Exp
{
	void CommandBuffer::Push(Mesh* mesh, Material* material, glm::mat4 transform)
	{
		RenderCommand command;
		command.Mesh = mesh;
		command.Material = material;
		command.Transform = transform;

		if (material->Type == Material::MaterialType::MATERIAL_DEFAULT)
		{
			DeferredCommands.push_back(command);
		}
		else if (material->Type == Material::MaterialType::MATERIAL_CUSTOM)
		{
			ForwardCommands.push_back(command);
		}

	}

	void CommandBuffer::Clear()
	{
		DeferredCommands.clear();
		ForwardCommands.clear();
	}

	void CommandBuffer::Sort()
	{
		std::sort(DeferredCommands.begin(), DeferredCommands.end(), [](const RenderCommand& a, const RenderCommand& b) -> bool
			{
				if (!a.Material->GetShader() || !b.Material->GetShader())
					return false;
				return a.Material->GetShader()->ID > b.Material->GetShader()->ID;
			});
	}

}

