#include "RenderingModule.h"
#include <iostream>
#include <Remotery/Remotery.h>
#include <stack>

#include "../Mesh/Mesh.h"
#include "Material.h"

namespace Exp
{
	RenderingModule::RenderingModule()
	{
	}


	RenderingModule::~RenderingModule()
	{
	}

	void RenderingModule::StartUp()
	{
		rmt_ScopedCPUSample(RenderingModuleStartUp, 0)
		std::cout << "RenderingModule StartUp" << std::endl;
	}

	void RenderingModule::Shutdown()
	{
		std::cout << "RenderingModule Shutdown" << std::endl;
	}

	void RenderingModule::PushMesh(SceneNode * Node)
	{
		Node->UpdateTransform(true);

		//Push the node and all the children for rendering
		std::stack<SceneNode*> StackNode;
		StackNode.push(Node);
		for (unsigned int i = 0; i < Node->GetChildCount(); ++i)
		{
			StackNode.push(Node->GetChildByIndex(i));
		}

		while (!StackNode.empty())
		{
			SceneNode * CurrentNode = StackNode.top();
			StackNode.pop();

			if (CurrentNode->Mesh && CurrentNode->Material)
			{
				PushMeshRenderCommand(CurrentNode->Mesh, CurrentNode->Material, CurrentNode->GetTransform());
			}

			for (unsigned int i = 0; i < CurrentNode->GetChildCount(); ++i)
			{
				StackNode.push(CurrentNode->GetChildByIndex(i));
			}
		}
	}

	void RenderingModule::Render()
	{
		rmt_ScopedCPUSample(Rendering, 0)

		m_geometryCommands.sort();

		// nothing to pass as GL doesn't have any contexts
		m_geometryCommands.submit(nullptr, true);
	}

	void RenderingModule::PushMeshRenderCommand(Mesh * mesh, Material * material, const glm::mat4 & transform)
	{
		auto key = cb::DrawKey::makeDefault(0, cb::ViewLayerType::e3D);
		key.setMaterial(1);
		if (mesh->Indices.size() > 0)
		{
			auto* cmd = m_geometryCommands.addCommand<cmds::DrawIndexed>(key);
			cmd->vao = mesh->VAO;
			cmd->base = 0;
			cmd->count = (uint32_t)mesh->Indices.size();
			cmd->primitive = mesh->Topology == Mesh::TOPOLOGY::TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES;
			cmd->useShortIndices = 0;
			//m_geometryCommands.materialBinder().materials.push_back(material);
			CB_DEBUG_COMMAND_TAG(cmd);
		}
		else
		{
			auto* cmd = m_geometryCommands.addCommand<cmds::DrawArrays>(key);
			cmd->vao = mesh->VAO;
			cmd->base = 0;
			cmd->count = (uint32_t)mesh->Positions.size();
			cmd->primitive = mesh->Topology == Mesh::TOPOLOGY::TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES;

			CB_DEBUG_COMMAND_TAG(cmd);
		}
		
	}

}

