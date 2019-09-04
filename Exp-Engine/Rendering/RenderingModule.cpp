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
		glEnable(GL_DEPTH_TEST);

		projUBOLocation = 0; // Fixed in shaders
		glGenBuffers(1, &projUBOId);
		glBindBuffer(GL_UNIFORM_BUFFER, projUBOId);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(ProjectionUBO), &projUBOData, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void RenderingModule::UpdateGlobalUBO()
	{
		if (RenderCamera)
		{
			projUBOData.m_viewMatrix = this->RenderCamera->m_view;
			projUBOData.m_projectionMatrix = this->RenderCamera->m_projection;
			projUBOData.m_viewPosition = this->RenderCamera->m_position;

			glGenBuffers(1, &projUBOId);
			glBindBuffer(GL_UNIFORM_BUFFER, projUBOId);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(ProjectionUBO), &projUBOData, GL_STREAM_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, projUBOId);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
	}

	void RenderingModule::Render(RenderCommand* command, bool updateGLSettings)
	{
		Mesh* mesh = command->Mesh;
		Material* material = command->Material;
		glm::mat4 transform = command->Transform;
		Shader* currentShader = material->GetShader();

		if (updateGLSettings)
		{
			GLCache::getInstance().SetBlend(material->Blend);
			if (material->Blend)
			{
				GLCache::getInstance().SetBlendFunc(material->BlendSrc, material->BlendDst);
			}
			GLCache::getInstance().SetDepthFunc(material->DepthCompare);
			GLCache::getInstance().SetDepthTest(material->DepthTest);
			GLCache::getInstance().SetCull(material->Cull);
			GLCache::getInstance().SetCullFace(material->CullFace);
		}

		if (!currentShader)
			return;

		currentShader->use();
		currentShader->setMat4("model", transform);

		// bind/active uniform sampler/texture objects
		auto samplers = material->GetSamplerUniforms();
		for (auto it = samplers.begin(); it != samplers.end(); ++it)
		{
			it->second.Texture->Bind(it->second.Unit);
		}

		auto uniforms = material->GetUniforms();
		for (auto it = uniforms.begin(); it != uniforms.end(); ++it)
		{
			switch (it->second.Type)
			{
			case SHADER_TYPE_INT:
				currentShader->setInt(it->first, it->second.Int);
				break;
			case SHADER_TYPE_FLOAT:
				currentShader->setFloat(it->first, it->second.Float);
				break;
			case SHADER_TYPE_VEC3:
				currentShader->setVec3(it->first, it->second.Vec3);
				break;
			default:
				break;
			}
		}

		RenderMesh(mesh);
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

		glClearColor(0.1f, 0.1f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now

		UpdateGlobalUBO();

		m_CommandBuffer.Sort();

		for (auto& CurrentRenderCommand : m_CommandBuffer.DeferredCommands)
		{
			Render(&CurrentRenderCommand, true);
		}

		m_CommandBuffer.Clear();
	}

	void RenderingModule::PushMeshRenderCommand(Mesh * mesh, Material * material, const glm::mat4 & transform)
	{
		m_CommandBuffer.Push(mesh, material, transform);
		
	}

	void RenderingModule::RenderMesh(Mesh* mesh)
	{
		glBindVertexArray(mesh->VAO);
		if (mesh->Indices.size() > 0)
		{
			glDrawElements(mesh->Topology == Mesh::TOPOLOGY::TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, mesh->Indices.size(), GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawArrays(mesh->Topology == Mesh::TOPOLOGY::TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, 0, mesh->Positions.size());
		}
	}
}

