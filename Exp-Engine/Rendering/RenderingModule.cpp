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

#include "../Scene/Skybox.h"


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

	void RenderingModule::DisplayUI()
	{
		if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Wireframe", &m_Wireframe);
			if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::AlignTextToFramePadding();
				bool treeopen = ImGui::TreeNodeEx("Directional Lights", ImGuiTreeNodeFlags_AllowItemOverlap);
				ImGui::SameLine();
				if (ImGui::Button("+"))
				{
					AddDirectionalLight(glm::vec3(0.0f, 1.0f, 0.0f));
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Add New Directional Light");


				if (treeopen)
				{
					for (int i = 0; i < m_DirectionalLights.size(); ++i)
					{
						ImGui::Separator();

						ImGui::Text(std::string("Directional Light " + std::to_string(i)).c_str());
						ImGui::Checkbox(std::string("Visible###Dir" + std::to_string(i)).c_str(), &m_DirectionalLights[i]->m_Visible);
						ImGui::ColorEdit3(std::string("Color###Dir" + std::to_string(i)).c_str(), &m_DirectionalLights[i]->m_Color[0]);
						ImGui::SliderFloat3(std::string("Direction###Dir" + std::to_string(i)).c_str(), &m_DirectionalLights[i]->m_Direction[0], -1.0f, 1.0f);
						ImGui::SliderFloat(std::string("Intensity###Dir" + std::to_string(i)).c_str(), &m_DirectionalLights[i]->m_Intensity, 0.0f, 5.0f);
						ImGui::Checkbox(std::string("Shadow###Dir" + std::to_string(i)).c_str(), &m_DirectionalLights[i]->m_CastShadows);
						ImGui::Checkbox(std::string("Debug Mesh###Dir" + std::to_string(i)).c_str(), &m_DirectionalLights[i]->m_RenderMesh);
					}
					
					ImGui::TreePop();
				}
				
			}
		}
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
		m_CustomTarget = std::make_shared<RenderTarget>(1, 1, GL_HALF_FLOAT, 1, true);
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
			if (it->second.Type == SHADER_TYPE_SAMPLERCUBE)
				it->second.TextureCube->Bind(it->second.Unit);
			else
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

	DirectionalLight * RenderingModule::AddDirectionalLight(glm::vec3 Direction)
	{
		std::shared_ptr<DirectionalLight> Light = std::make_shared<DirectionalLight>();
		Light->m_Direction = Direction;
		m_DirectionalLights.push_back(Light);
		return Light.get();
	}

	void RenderingModule::SetCamera(Camera* Camera)
	{
		if (RenderCamera != Camera)
		{
			RenderCamera = Camera;
		}
	}

	void RenderingModule::SetSkybox(std::string folder)
	{
		CurrentSkybox = std::make_shared<Skybox>();
		CurrentSkybox->SetCubemap(Resources::LoadTextureCube("skybox", folder));
	}

	void RenderingModule::RenderSkybox()
	{
		if (CurrentSkybox.get())
		{
			GLCache::getInstance().SetPolygonMode(m_Wireframe ? GL_LINE : GL_FILL);

			RenderCommand command;
			command.Mesh = CurrentSkybox->Mesh;
			command.Material = CurrentSkybox->Material;
			command.Transform = CurrentSkybox->GetTransform();
			Render(&command, true);

			GLCache::getInstance().SetPolygonMode(GL_FILL);
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

		// set default GL state
		GLCache::getInstance().Reset();

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// 4. Render deferred shader for each light
		GLCache::getInstance().SetDepthTest(false);
		GLCache::getInstance().SetBlend(true);
		GLCache::getInstance().SetBlendFunc(GL_ONE, GL_ONE);

		// bind gbuffer
		m_GBuffer->GetColorTexture(0)->Bind(0);
		m_GBuffer->GetColorTexture(1)->Bind(1);
		m_GBuffer->GetColorTexture(2)->Bind(2);

		if (true/*Lights*/)
		{
			// directional lights
			for (auto it = m_DirectionalLights.begin(); it != m_DirectionalLights.end(); ++it)
			{
				RenderDeferredDirLight((*it).get());
			}
			// point lights
			//m_GLCache.SetCullFace(GL_FRONT);
			//for (auto it = m_PointLights.begin(); it != m_PointLights.end(); ++it)
			//{
			//	// only render point lights if within frustum
			//	if (m_Camera->Frustum.Intersect((*it)->Position, (*it)->Radius))
			//	{
			//		renderDeferredPointLight(*it);
			//	}
			//}
			//m_GLCache.SetCullFace(GL_BACK);
		}

		GLCache::getInstance().SetDepthTest(true);
		GLCache::getInstance().SetBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		GLCache::getInstance().SetBlend(false);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer->ID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		glBlitFramebuffer(0, 0, m_GBuffer->Width, m_GBuffer->Height, 0, 0, Exp::WinParameters.screenWidth, Exp::WinParameters.screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		RenderSkybox();
		
		// Debug GBuffer
		//GLCache::getInstance().SetPolygonMode(GL_LINE);
		/*glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		Blit(m_GBuffer->GetColorTexture(1));*/

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
			glDrawElements(mesh->Topology == Mesh::TOPOLOGY::TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, (GLsizei)mesh->Indices.size(), GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawArrays(mesh->Topology == Mesh::TOPOLOGY::TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, 0, (GLsizei)mesh->Positions.size());
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

	void RenderingModule::RenderDeferredDirLight(DirectionalLight* light)
	{
		Shader* dirShader = nullptr;
		//TODO refacto this Post Module Initialize
		if (MaterialLibraryModule * MatLibraryModule = ModuleManager::Get().GetModule<MaterialLibraryModule>("MaterialLibrary"))
		{
			dirShader = MatLibraryModule->GetShader("deferredDirectional");
		}

		if (dirShader)
		{
			dirShader->use();
			dirShader->setVec3("lightDir", light->m_Direction);
			dirShader->setVec3("lightColor", glm::normalize(light->m_Color) * light->m_Intensity);

			dirShader->setInt("gPosition", 0);
			dirShader->setInt("gNormal", 1);
			dirShader->setInt("gAlbedoSpec", 2);

			if (light->m_ShadowMapRT)
			{
				/*dirShader->SetMatrix("lightShadowViewProjection", light->LightSpaceViewProjection);
				light->ShadowMapRT->GetDepthStencilTexture()->Bind(3);*/
			}

			RenderMesh(m_NDCPlane.get());
		}
		
	}
}

