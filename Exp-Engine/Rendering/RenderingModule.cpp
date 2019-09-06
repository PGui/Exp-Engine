#include "RenderingModule.h"
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <Remotery/Remotery.h>

#include "Material.h"

#include "../Module/ModuleManager.h"
#include "../Core/EngineModule.h"
#include "../MaterialLibrary/MaterialLibraryModule.h"
#include "../Camera/Camera.h"
#include "../Mesh/Mesh.h"
#include "../Core/WindowParameters.h"


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

		// Initialize Deferred Rendering
		m_GBuffer = std::make_shared<RenderTarget>(Exp::WinParameters.screenWidth, Exp::WinParameters.screenHeight, GL_HALF_FLOAT, 3, true);
		m_NDCPlane = std::make_shared<Quad>(1.0f, 1.0f);
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

	std::shared_ptr<RenderTarget> RenderingModule::GetGBuffer()
	{
		return m_GBuffer;
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

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now

		UpdateGlobalUBO();

		m_CommandBuffer.Sort();

		glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer->ID);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);
		
		GLCache::getInstance().SetPolygonMode(m_Wireframe ? GL_LINE : GL_FILL);

		for (auto& CurrentRenderCommand : m_CommandBuffer.DeferredCommands)
		{
			Render(&CurrentRenderCommand, true);
		}

		GLCache::getInstance().SetPolygonMode(GL_FILL);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		Blit(m_GBuffer->GetColorTexture(1));

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

	void RenderingModule::Blit(Texture* src, RenderTarget* dst, Material* material, std::string textureUniformName)
	{
		// if a destination target is given, bind to its framebuffer
		if (dst)
		{
			glViewport(0, 0, dst->Width, dst->Height);
			glBindFramebuffer(GL_FRAMEBUFFER, dst->ID);
			/*if (dst->HasDepthAndStencil)
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			else*/
				glClear(GL_COLOR_BUFFER_BIT);
		}
		// else we bind to the default framebuffer
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glViewport(0, 0, m_RenderSize.x, m_RenderSize.y);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}
		// if no material is given, use default blit material
		if (!material)
		{
			MaterialLibraryModule* MatLibraryModule = ModuleManager::Get().GetModule<MaterialLibraryModule>("MaterialLibrary");
			if (MatLibraryModule)
			{
				material = MatLibraryModule->GetMaterial("blit");
			}
			
		}
		// if a source render target is given, use its color buffer as input to material shader.
		if (material && src)
		{
			material->SetTexture(textureUniformName, src, 0);
		}
		// render screen-space material to quad which will be displayed in dst's buffers.
		RenderCommand command;
		command.Material = material;
		command.Mesh = m_NDCPlane.get();
		Render(&command, true);
	}
}

