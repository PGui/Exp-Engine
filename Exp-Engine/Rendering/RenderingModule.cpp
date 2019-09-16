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

		spdlog::info("RenderingModule StartUp");
	}

	void RenderingModule::Shutdown()
	{
		spdlog::info("RenderingModule Shutdown");
	}

	void RenderingModule::DisplayUI()
	{
		if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Wireframe", &m_Wireframe);
			ImGui::Checkbox("Lights", &m_DisplayLights);
			ImGui::Checkbox("Skybox", &m_DisplaySkybox);
			if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::AlignTextToFramePadding();
				bool dirtreeopen = ImGui::TreeNodeEx("Directional Lights", ImGuiTreeNodeFlags_AllowItemOverlap);
				ImGui::SameLine();
				if (ImGui::Button("+"))
				{
					AddDirectionalLight(glm::vec3(0.0f, 1.0f, 0.0f));
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Add New Directional Light");


				if (dirtreeopen)
				{
					for (int i = 0; i < m_DirectionalLights.size(); ++i)
					{
						ImGui::Separator();

						ImGui::Text(		std::string("Directional Light "    + std::to_string(i)).c_str());
						ImGui::SameLine();
						if (ImGui::Button("-"))
						{
							//Remove DL
						}
						ImGui::Checkbox(	std::string("Visibility###VisDir"	+ std::to_string(i)).c_str(), &m_DirectionalLights[i]->m_Visible);
						ImGui::ColorEdit3(	std::string("Color###ColDir"		+ std::to_string(i)).c_str(), &m_DirectionalLights[i]->m_Color[0]);
						ImGui::SliderFloat3(std::string("Direction###DirDir"	+ std::to_string(i)).c_str(), &m_DirectionalLights[i]->m_Direction[0], -1.0f, 1.0f);
						ImGui::SliderFloat(	std::string("Intensity###IntDir"	+ std::to_string(i)).c_str(), &m_DirectionalLights[i]->m_Intensity, 0.0f, 5.0f);
						ImGui::Checkbox(	std::string("Shadow###ShaDir"		+ std::to_string(i)).c_str(), &m_DirectionalLights[i]->m_CastShadows);
						ImGui::Checkbox(	std::string("Debug Mesh###DebDir"	+ std::to_string(i)).c_str(), &m_DirectionalLights[i]->m_RenderMesh);
					}
					
					ImGui::TreePop();
				}

				//Points lights
				ImGui::AlignTextToFramePadding();
				bool pointtreeopen = ImGui::TreeNodeEx("Point Lights", ImGuiTreeNodeFlags_AllowItemOverlap);
				ImGui::SameLine();
				if (ImGui::Button("+"))
				{
					AddPointLight();
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Add New Point Light");


				if (pointtreeopen)
				{
					int index = 0;
					for (int i = 0; i < m_PointLights.size(); ++i)
					{
						ImGui::Separator();
						

						ImGui::Text(std::string("Point Light " + std::to_string(i)).c_str());
						ImGui::SameLine();
						if (ImGui::Button("-"))
						{
							//Remove PL
						}
						ImGui::Checkbox(std::string("Visibility##" + std::to_string(i + index++)).c_str(), &m_PointLights[i]->m_Visible);
						ImGui::ColorEdit3(std::string("Color##" + std::to_string(i + index++)).c_str(), &m_PointLights[i]->m_Color[0]);
						ImGui::SliderFloat3(std::string("Position##" + std::to_string(i + index++)).c_str(), &m_PointLights[i]->m_Position[0],-50.0f, 50.0f);
						ImGui::SliderFloat(std::string("Radius##" + std::to_string(i + index++)).c_str(), &m_PointLights[i]->m_Radius, 0.0f, 50.0f);
						ImGui::SliderFloat(std::string("Intensity##" + std::to_string(i + index++)).c_str(), &m_PointLights[i]->m_Intensity, 0.0f, 5.0f);
						ImGui::Checkbox(std::string("Shadow##" + std::to_string(i + index++)).c_str(), &m_PointLights[i]->m_CastShadows);
						ImGui::Checkbox(std::string("Debug Mesh##" + std::to_string(i + index++)).c_str(), &m_PointLights[i]->m_RenderMesh);
					}

					ImGui::TreePop();
				}
				
			}
		}

		if (ImGui::CollapsingHeader("Shaders"/*, ImGuiTreeNodeFlags_DefaultOpen*/))
		{
			for (auto it = Resources::DebugGetShaders().begin(); it != Resources::DebugGetShaders().end(); ++it)
			{
				ImGui::Text(it->second.Name.c_str());
				ImGui::SameLine();
				if (ImGui::Button(std::string("Reload###"+ it->second.Name).c_str()))
				{
					spdlog::info("Reloading {}...", it->second.Name);
					Resources::ReloadShader(it->second.Name);
				}
			}
		}
	}

	void RenderingModule::PostInitialize()
	{
		m_MaterialLibrary= ModuleManager::Get().GetModule<MaterialLibraryModule>("MaterialLibrary");
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
		m_GBuffer = std::make_shared<RenderTarget>(m_RenderSize.x, m_RenderSize.y, GL_HALF_FLOAT, 3, true);
		m_CustomTarget = std::make_shared<RenderTarget>(1, 1, GL_HALF_FLOAT, 1, true);
		m_NDCPlane = std::make_shared<Quad>(1.0f, 1.0f);
		m_PointLightSphere = std::make_shared<Sphere>(16, 16);

		// Debug Lighting
		m_DebugLightMesh = std::make_shared<Cube>();

		GLCache::getInstance().Reset(true);
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

	PointLight* RenderingModule::AddPointLight(glm::vec3 Position, float Radius)
	{
		std::shared_ptr<PointLight> Light = std::make_shared<PointLight>();
		Light->m_Position = Position;
		Light->m_Radius = Radius;
		m_PointLights.push_back(Light);
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

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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

		if (m_DisplayLights)
		{
			//GLCache::getInstance().SetCull(false);//TODO Remove - DEBUG
			// directional lights
			for (auto it = m_DirectionalLights.begin(); it != m_DirectionalLights.end(); ++it)
			{
				RenderDeferredDirLight((*it).get());
			}
			// point lights

////////////////////////////////////////////////////////////////
			// Pre point light
			/*for (auto it = m_PointLights.begin(); it != m_PointLights.end(); ++it)
			{
				PreRenderDeferredPointLight((*it).get());
			}


			GLCache::getInstance().SetCullFace(GL_FRONT);
			for (auto it = m_PointLights.begin(); it != m_PointLights.end(); ++it)
			{
				RenderDeferredPointLight((*it).get());
			}
			GLCache::getInstance().SetCullFace(GL_BACK);*/
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
			GLCache::getInstance().SetCullFace(GL_FRONT);
			for (auto it = m_PointLights.begin(); it != m_PointLights.end(); ++it)
			{
				// only render point lights if within frustum
				if (true/*m_Camera->Frustum.Intersect((*it)->m_Position, (*it)->m_Radius)*/)
				{
					RenderDeferredPointLight((*it).get());
				}
			}
			GLCache::getInstance().SetCullFace(GL_BACK);
////////////////////////////////////////////////////////////////
		}

		GLCache::getInstance().SetDepthTest(true);
		GLCache::getInstance().SetBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		GLCache::getInstance().SetBlend(false);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer->ID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		glBlitFramebuffer(0, 0, m_GBuffer->Width, m_GBuffer->Height, 0, 0, m_RenderSize.x, m_RenderSize.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (m_DisplaySkybox)
			RenderSkybox();


		RenderDebugLights();
		
		// Debug GBuffer
		//GLCache::getInstance().SetPolygonMode(GL_LINE);
		/*glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		Blit(m_GBuffer->GetDepthStencilTexture());*/

		m_CommandBuffer.Clear();
	}

	void RenderingModule::ResizeRenderer(int Width, int Height)
	{
		m_RenderSize = glm::vec2(Width, Height);
		m_GBuffer->Resize(Width, Height);
		if (CurrentSkybox.get())
		{
			CurrentSkybox->GetCubemap()->Resize(Width, Height);
		}
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
			if (dst->bHasDepthAndStencil)
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			else
				glClear(GL_COLOR_BUFFER_BIT);
		}
		// else we bind to the default framebuffer
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, m_RenderSize.x, m_RenderSize.y);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}
		// if no material is given, use default blit material
		if (!material)
		{
			if (m_MaterialLibrary)
			{
				material = m_MaterialLibrary->GetMaterial("blit");
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
		if (!light->m_Visible)
			return;
		Shader* dirShader = nullptr;
		if (m_MaterialLibrary)
		{
			dirShader = m_MaterialLibrary->GetShader("deferredDirectional");
			if (dirShader)//TODO put as member
			{
				dirShader->use();
				dirShader->setVec3("lightDir", light->m_Direction);
				dirShader->setVec3("lightColor", /*glm::normalize*/(light->m_Color) * light->m_Intensity);

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

	void RenderingModule::PreRenderDeferredPointLight(PointLight* light)
	{
		Shader* stencilShader = nullptr;
		if (m_MaterialLibrary)
		{
			stencilShader = m_MaterialLibrary->GetShader("stencilLightShader");

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, light->m_Position);
			model = glm::scale(model, glm::vec3(light->m_Radius));

			//glBindTexture(GL_TEXTURE_2D, m_GBuffer->GetDepthStencilTexture()->ID);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderSize.x, m_RenderSize.y, 0, GL_RGB, GL_FLOAT, NULL);
			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_GBuffer->GetDepthStencilTexture()->ID, 0);

			//glEnable(GL_STENCIL_TEST);
			
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE);

			glEnable(GL_STENCIL_TEST);
			glDisable(GL_CULL_FACE);

			glStencilMask(0xFF);
			glClearStencil(0);

			// we'll render the light volume faces once per light
			glStencilFunc(GL_EQUAL, 0, 0xFF);
			glStencilOp(GL_KEEP, GL_INCR, GL_INCR);
			stencilShader->use();
			RenderMesh(m_PointLightSphere.get());

		/*	stencilShader->use();
			glDrawBuffer(GL_NONE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
			glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
			stencilShader->setMat4("model", model);
			RenderMesh(m_PointLightSphere.get());
*/
		}


	}

	void RenderingModule::RenderDeferredPointLight(PointLight* light)
	{
		if (!light->m_Visible)
			return;

		Shader* pointShader = nullptr;
		if (m_MaterialLibrary)
		{
			pointShader = m_MaterialLibrary->GetShader("deferredPoint");
			if (pointShader)
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, light->m_Position);
				model = glm::scale(model, glm::vec3(light->m_Radius));

				pointShader->use();
				pointShader->setVec3("lightPos", light->m_Position);
				pointShader->setVec3("lightColor",/* glm::normalize*/(light->m_Color) * light->m_Intensity);
				pointShader->setFloat("lightRadius", light->m_Radius);
				pointShader->setInt("gPosition", 0);
				pointShader->setInt("gNormal", 1);
				pointShader->setInt("gAlbedoSpec", 2);
				pointShader->setMat4("model", model);

				//GLCache::getInstance().SetPolygonMode(GL_LINE);
				RenderMesh(m_PointLightSphere.get());
				//GLCache::getInstance().SetPolygonMode(GL_FILL);
			}
		}
	}

	void RenderingModule::RenderDebugLights()
	{
		if (m_MaterialLibrary)
		{
			for (auto it = m_PointLights.begin(); it != m_PointLights.end(); ++it)
			{
				if ((*it)->m_RenderMesh)
				{
					RenderCommand command;
					command.Material = m_MaterialLibrary->GetMaterial("debugLight");
					command.Material->SetVector("lightColor", (*it)->m_Color * (*it)->m_Intensity);
					command.Mesh = m_DebugLightMesh.get();
					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, (*it)->m_Position);
					model = glm::scale(model, glm::vec3(0.25f));
					command.Transform = model;
					Render(&command, false);
				}
			}
			
		}
	}
}