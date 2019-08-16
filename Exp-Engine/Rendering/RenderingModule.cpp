#include "RenderingModule.h"
#include <iostream>
#include <Remotery/Remotery.h>
#include <stack>

#include "../Mesh/Mesh.h"
#include "Material.h"
#include "../Camera/Camera.h"

#include "CustomCommands.h"

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
		
		InitGL();

		std::cout << "RenderingModule StartUp" << std::endl;
	}

	void RenderingModule::Shutdown()
	{
		std::cout << "RenderingModule Shutdown" << std::endl;
	}

	void RenderingModule::InitGL()
	{
		projUBOLocation = 0; // Fixed in shaders
		glGenBuffers(1, &projUBOId);
		glBindBuffer(GL_UNIFORM_BUFFER, projUBOId);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(ProjUBO), &projUBOData, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void RenderingModule::UpdateGlobalUBO()
	{
		if (RenderCamera)
		{
			projUBOData.m_viewMatrix = this->RenderCamera->m_view;
			projUBOData.m_projectionMatrix = this->RenderCamera->m_projection;
			projUBOData.m_inverseViewMatrix = glm::inverse(RenderCamera->m_view);
			projUBOData.m_inverseProjMatrix = glm::inverse(RenderCamera->m_projection);

			const auto key = cb::DrawKey::makeCustom(cb::ViewLayerType::eHighest, 0);
			auto* cmd = m_geometryCommands.addCommand<BeginFrameCommand>(key);

			cmd->projUBOId = projUBOId;
			cmd->projUBOData = projUBOData;

			cmd->materialBinder = &m_geometryCommands.materialBinder();
			CB_DEBUG_COMMAND_TAG(cmd);
		}
	}

	void RenderingModule::SetCamera(Camera* Camera)
	{
		if (RenderCamera != Camera)
		{
			RenderCamera = Camera;
		}
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

		UpdateGlobalUBO();

		m_geometryCommands.sort();

		// nothing to pass as GL doesn't have any contexts
		m_geometryCommands.submit(nullptr, true);
	}

	void RenderingModule::PushMeshRenderCommand(Mesh * mesh, Material * material, const glm::mat4 & transform)
	{
		auto key = cb::DrawKey::makeDefault(0, cb::ViewLayerType::e3D);
		key.setMaterial(material->MaterialId);
		m_geometryCommands.materialBinder().useMaterial(material);
		if (mesh->Indices.size() > 0)
		{
			auto* cmd = m_geometryCommands.addCommand<cmds::DrawIndexed>(key);
			cmd->vao = mesh->VAO;
			cmd->base = 0;
			cmd->count = (uint32_t)mesh->Indices.size();
			cmd->primitive = mesh->Topology == Mesh::TOPOLOGY::TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES;
			cmd->useShortIndices = 0;
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

