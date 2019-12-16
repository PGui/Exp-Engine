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
			ImGui::Checkbox("Wireframe", &wireframe);
			ImGui::Checkbox("Lights", &displayLights);
			ImGui::Checkbox("Skybox", &displaySkybox);
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
					for (int i = 0; i < directionalLights.size(); ++i)
					{
						ImGui::Separator();

						ImGui::Text(		std::string("Directional Light "    + std::to_string(i)).c_str());
						ImGui::SameLine();
						if (ImGui::Button("-"))
						{
							//Remove DL
						}
						ImGui::Checkbox(	std::string("Visibility###VisDir"	+ std::to_string(i)).c_str(), &directionalLights[i]->visible);
						ImGui::ColorEdit3(	std::string("Color###ColDir"		+ std::to_string(i)).c_str(), &directionalLights[i]->color[0]);
						ImGui::SliderFloat3(std::string("Direction###DirDir"	+ std::to_string(i)).c_str(), &directionalLights[i]->direction[0], -1.0f, 1.0f);
						ImGui::SliderFloat(	std::string("Intensity###IntDir"	+ std::to_string(i)).c_str(), &directionalLights[i]->intensity, 0.0f, 5.0f);
						ImGui::Checkbox(	std::string("Shadow###ShaDir"		+ std::to_string(i)).c_str(), &directionalLights[i]->castShadows);
						ImGui::Checkbox(	std::string("Debug Mesh###DebDir"	+ std::to_string(i)).c_str(), &directionalLights[i]->renderMesh);
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
					for (int i = 0; i < pointLights.size(); ++i)
					{
						ImGui::Separator();
						

						ImGui::Text(std::string("Point Light " + std::to_string(i)).c_str());
						ImGui::SameLine();
						if (ImGui::Button("-"))
						{
							//Remove PL
						}
						ImGui::Checkbox(std::string("Visibility##" + std::to_string(i + index++)).c_str(), &pointLights[i]->visible);
						ImGui::ColorEdit3(std::string("Color##" + std::to_string(i + index++)).c_str(), &pointLights[i]->color[0]);
						ImGui::SliderFloat3(std::string("Position##" + std::to_string(i + index++)).c_str(), &pointLights[i]->position[0],-50.0f, 50.0f);
						ImGui::SliderFloat(std::string("Radius##" + std::to_string(i + index++)).c_str(), &pointLights[i]->radius, 0.0f, 50.0f);
						ImGui::SliderFloat(std::string("Intensity##" + std::to_string(i + index++)).c_str(), &pointLights[i]->intensity, 0.0f, 5.0f);
						ImGui::Checkbox(std::string("Shadow##" + std::to_string(i + index++)).c_str(), &pointLights[i]->castShadows);
						ImGui::Checkbox(std::string("Debug Mesh##" + std::to_string(i + index++)).c_str(), &pointLights[i]->renderMesh);
					}

					ImGui::TreePop();
				}
				
			}
		}

		if (ImGui::CollapsingHeader("Shaders"/*, ImGuiTreeNodeFlags_DefaultOpen*/))
		{
			for (auto it = Resources::DebugGetShaders().begin(); it != Resources::DebugGetShaders().end(); ++it)
			{
				ImGui::Text(it->second.name.c_str());
				ImGui::SameLine();
				if (ImGui::Button(std::string("Reload###"+ it->second.name).c_str()))
				{
					spdlog::info("Reloading {}...", it->second.name);
					Resources::ReloadShader(it->second.name);
				}
			}
		}
	}

	void RenderingModule::PostInitialize()
	{
		materialLibrary= ModuleManager::Get().GetModule<MaterialLibraryModule>("MaterialLibrary");
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
		GBuffer = std::make_shared<RenderTarget>(renderSize.x, renderSize.y, GL_HALF_FLOAT, 3, true);
		customTarget = std::make_shared<RenderTarget>(1, 1, GL_HALF_FLOAT, 1, true);
		ndcPlane = std::make_shared<Quad>(1.0f, 1.0f);
		pointLightSphere = std::make_shared<Sphere>(16, 16);

		// Debug Lighting
		debugLightMesh = std::make_shared<Cube>();

		GLCache::GetInstance().Reset(true);
	}

	void RenderingModule::UpdateGlobalUBO()
	{
		if (renderCamera)
		{
			projUBOData.viewMatrix = this->renderCamera->view;
			projUBOData.projectionMatrix = this->renderCamera->projection;
			projUBOData.viewPosition = this->renderCamera->position;

			glGenBuffers(1, &projUBOId);
			glBindBuffer(GL_UNIFORM_BUFFER, projUBOId);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(ProjectionUBO), &projUBOData, GL_STREAM_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, projUBOId);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
	}

	void RenderingModule::Render(RenderCommand* command, bool updateGLSettings)
	{
		Mesh* mesh = command->mesh;
		Material* material = command->material;
		glm::mat4 transform = command->transform;
		Shader* currentShader = material->GetShader();

		if (updateGLSettings)
		{
			GLCache::GetInstance().SetBlend(material->blend);
			if (material->blend)
			{
				GLCache::GetInstance().SetBlendFunc(material->blendSrc, material->blendDst);
			}
			GLCache::GetInstance().SetDepthFunc(material->depthCompare);
			GLCache::GetInstance().SetDepthTest(material->depthTest);
			GLCache::GetInstance().SetCull(material->cull);
			GLCache::GetInstance().SetCullFace(material->cullFace);
		}

		if (!currentShader)
			return;

		currentShader->Use();
		currentShader->SetMat4("model", transform);

		// bind/active uniform sampler/texture objects
		auto samplers = material->GetSamplerUniforms();
		for (auto it = samplers.begin(); it != samplers.end(); ++it)
		{
			if (it->second.type == SHADER_TYPE_SAMPLERCUBE)
				it->second.TextureCube->Bind(it->second.Unit);
			else
				it->second.Texture->Bind(it->second.Unit);
		}

		auto uniforms = material->GetUniforms();
		for (auto it = uniforms.begin(); it != uniforms.end(); ++it)
		{
			switch (it->second.type)
			{
			case SHADER_TYPE_INT:
				currentShader->SetInt(it->first, it->second.Int);
				break;
			case SHADER_TYPE_FLOAT:
				currentShader->SetFloat(it->first, it->second.Float);
				break;
			case SHADER_TYPE_VEC3:
				currentShader->SetVec3(it->first, it->second.Vec3);
				break;
			default:
				break;
			}
		}

		RenderMesh(mesh);
	}

	std::shared_ptr<RenderTarget> RenderingModule::GetGBuffer()
	{
		return GBuffer;
	}

	DirectionalLight * RenderingModule::AddDirectionalLight(glm::vec3 Direction)
	{
		std::shared_ptr<DirectionalLight> Light = std::make_shared<DirectionalLight>();
		Light->direction = Direction;
		directionalLights.push_back(Light);
		return Light.get();
	}

	PointLight* RenderingModule::AddPointLight(glm::vec3 Position, float Radius)
	{
		std::shared_ptr<PointLight> Light = std::make_shared<PointLight>();
		Light->position = Position;
		Light->radius = Radius;
		pointLights.push_back(Light);
		return Light.get();
	}

	void RenderingModule::SetCamera(Camera* Camera)
	{
		if (renderCamera != Camera)
		{
			renderCamera = Camera;
		}
	}

	void RenderingModule::SetSkybox(std::string folder)
	{
		currentSkybox = std::make_shared<Skybox>();
		currentSkybox->SetCubemap(Resources::LoadTextureCube("skybox", folder));
	}

	void RenderingModule::RenderSkybox()
	{
		if (currentSkybox.get())
		{
			GLCache::GetInstance().SetPolygonMode(wireframe ? GL_LINE : GL_FILL);

			RenderCommand command;
			command.mesh = currentSkybox->mesh;
			command.material = currentSkybox->material;
			command.transform = currentSkybox->GetTransform();
			Render(&command, true);

			GLCache::GetInstance().SetPolygonMode(GL_FILL);
		}
	}

	void RenderingModule::PushMesh(SceneNode * Node)
	{
		Node->UpdateTransform(true);

		//Push the node and all the children for rendering
		std::stack<SceneNode*> stackNode;
		stackNode.push(Node);
		for (unsigned int i = 0; i < Node->GetChildCount(); ++i)
		{
			stackNode.push(Node->GetChildByIndex(i));
		}

		while (!stackNode.empty())
		{
			SceneNode * currentNode = stackNode.top();
			stackNode.pop();

			if (currentNode->mesh && currentNode->material)
			{
				PushMeshRenderCommand(currentNode->mesh, currentNode->material, currentNode->GetTransform());
			}

			for (unsigned int i = 0; i < currentNode->GetChildCount(); ++i)
			{
				stackNode.push(currentNode->GetChildByIndex(i));
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
		GLCache::GetInstance().Reset();
		

		commandBuffer.Sort();

		glBindFramebuffer(GL_FRAMEBUFFER, GBuffer->ID);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		GLCache::GetInstance().SetPolygonMode(wireframe ? GL_LINE : GL_FILL);

		for (auto& currentRenderCommand : commandBuffer.deferredCommands)
		{
			Render(&currentRenderCommand, true);
		}

		

		GLCache::GetInstance().SetPolygonMode(GL_FILL);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// 4. Render deferred shader for each light
		GLCache::GetInstance().SetDepthTest(false);
		GLCache::GetInstance().SetBlend(true);
		GLCache::GetInstance().SetBlendFunc(GL_ONE, GL_ONE);

		// bind gbuffer
		GBuffer->GetColorTexture(0)->Bind(0);
		GBuffer->GetColorTexture(1)->Bind(1);
		GBuffer->GetColorTexture(2)->Bind(2);

		if (displayLights)
		{
			//GLCache::GetInstance().SetCull(false);//TODO Remove - DEBUG
			// directional lights
			for (auto it = directionalLights.begin(); it != directionalLights.end(); ++it)
			{
				RenderDeferredDirLight((*it).get());
			}
			// point lights

////////////////////////////////////////////////////////////////
			// Pre point light
			/*for (auto it = pointLights.begin(); it != pointLights.end(); ++it)
			{
				PreRenderDeferredPointLight((*it).get());
			}


			GLCache::GetInstance().SetCullFace(GL_FRONT);
			for (auto it = pointLights.begin(); it != pointLights.end(); ++it)
			{
				RenderDeferredPointLight((*it).get());
			}
			GLCache::GetInstance().SetCullFace(GL_BACK);*/
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
			GLCache::GetInstance().SetCullFace(GL_FRONT);
			for (auto it = pointLights.begin(); it != pointLights.end(); ++it)
			{
				// only render point lights if within frustum
				if (true/*camera->Frustum.Intersect((*it)->position, (*it)->radius)*/)
				{
					RenderDeferredPointLight((*it).get());
				}
			}
			GLCache::GetInstance().SetCullFace(GL_BACK);
////////////////////////////////////////////////////////////////
		}

		GLCache::GetInstance().SetDepthTest(true);
		GLCache::GetInstance().SetBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		GLCache::GetInstance().SetBlend(false);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, GBuffer->ID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		glBlitFramebuffer(0, 0, GBuffer->Width, GBuffer->Height, 0, 0, renderSize.x, renderSize.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (displaySkybox)
			RenderSkybox();


		RenderDebugLights();
		
		// Debug GBuffer
		//GLCache::GetInstance().SetPolygonMode(GL_LINE);
		/*glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		Blit(GBuffer->GetDepthStencilTexture());*/

		commandBuffer.Clear();
	}

	void RenderingModule::ResizeRenderer(int width, int height)
	{
		renderSize = glm::vec2(width, height);
		GBuffer->Resize(width, height);
		if (currentSkybox.get())
		{
			currentSkybox->GetCubemap()->Resize(width, height);
		}
	}

	void RenderingModule::PushMeshRenderCommand(Mesh * mesh, Material * material, const glm::mat4 & transform)
	{
		commandBuffer.Push(mesh, material, transform);
	}

	void RenderingModule::RenderMesh(Mesh* mesh)
	{
		glBindVertexArray(mesh->vao);
		if (mesh->indices.size() > 0)
		{
			glDrawElements(mesh->topology == Mesh::TOPOLOGY::TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, (GLsizei)mesh->indices.size(), GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawArrays(mesh->topology == Mesh::TOPOLOGY::TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, 0, (GLsizei)mesh->positions.size());
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
			glViewport(0, 0, renderSize.x, renderSize.y);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}
		// if no material is given, use default blit material
		if (!material)
		{
			if (materialLibrary)
			{
				material = materialLibrary->GetMaterial("blit");
			}
			
		}
		// if a source render target is given, use its color buffer as input to material shader.
		if (material && src)
		{
			material->SetTexture(textureUniformName, src, 0);
		}
		// render screen-space material to quad which will be displayed in dst's buffers.
		RenderCommand command;
		command.material = material;
		command.mesh = ndcPlane.get();
		Render(&command, true);
	}

	void RenderingModule::RenderDeferredDirLight(DirectionalLight* light)
	{
		if (!light->visible)
			return;
		Shader* dirShader = nullptr;
		if (materialLibrary)
		{
			dirShader = materialLibrary->GetShader("deferredDirectional");
			if (dirShader)//TODO put as member
			{
				dirShader->Use();
				dirShader->SetVec3("lightDir", light->direction);
				dirShader->SetVec3("lightColor", /*glm::normalize*/(light->color) * light->intensity);

				dirShader->SetInt("gPosition", 0);
				dirShader->SetInt("gNormal", 1);
				dirShader->SetInt("gAlbedoSpec", 2);

				if (light->shadowMapRT)
				{
					/*dirShader->SetMatrix("lightShadowViewProjection", light->LightSpaceViewProjection);
					light->ShadowMapRT->GetDepthStencilTexture()->Bind(3);*/
				}

				RenderMesh(ndcPlane.get());

			}
		}
	}

	void RenderingModule::PreRenderDeferredPointLight(PointLight* light)
	{
		Shader* stencilShader = nullptr;
		if (materialLibrary)
		{
			stencilShader = materialLibrary->GetShader("stencilLightShader");

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, light->position);
			model = glm::scale(model, glm::vec3(light->radius));

			//glBindTexture(GL_TEXTURE_2D, GBuffer->GetDepthStencilTexture()->id);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderSize.x, renderSize.y, 0, GL_RGB, GL_FLOAT, NULL);
			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, GBuffer->GetDepthStencilTexture()->id, 0);

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
			stencilShader->Use();
			RenderMesh(pointLightSphere.get());

		/*	stencilShader->Use();
			glDrawBuffer(GL_NONE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
			glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
			stencilShader->SetMat4("model", model);
			RenderMesh(pointLightSphere.get());
*/
		}


	}

	void RenderingModule::RenderDeferredPointLight(PointLight* light)
	{
		if (!light->visible)
			return;

		Shader* pointShader = nullptr;
		if (materialLibrary)
		{
			pointShader = materialLibrary->GetShader("deferredPoint");
			if (pointShader)
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, light->position);
				model = glm::scale(model, glm::vec3(light->radius));

				pointShader->Use();
				pointShader->SetVec3("lightPos", light->position);
				pointShader->SetVec3("lightColor",/* glm::normalize*/(light->color) * light->intensity);
				pointShader->SetFloat("lightRadius", light->radius);
				pointShader->SetInt("gPosition", 0);
				pointShader->SetInt("gNormal", 1);
				pointShader->SetInt("gAlbedoSpec", 2);
				pointShader->SetMat4("model", model);

				//GLCache::GetInstance().SetPolygonMode(GL_LINE);
				RenderMesh(pointLightSphere.get());
				//GLCache::GetInstance().SetPolygonMode(GL_FILL);
			}
		}
	}

	void RenderingModule::RenderDebugLights()
	{
		if (materialLibrary)
		{
			for (auto it = pointLights.begin(); it != pointLights.end(); ++it)
			{
				if ((*it)->renderMesh)
				{
					RenderCommand command;
					command.material = materialLibrary->GetMaterial("debugLight");
					command.material->SetVector("lightColor", (*it)->color * (*it)->intensity);
					command.mesh = debugLightMesh.get();
					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, (*it)->position);
					model = glm::scale(model, glm::vec3(0.25f));
					command.transform = model;
					Render(&command, false);
				}
			}
			
		}
	}
}