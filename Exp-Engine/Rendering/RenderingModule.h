#pragma once
#include "../Module/ModuleInterface.h"
#include "../CommandBuffer/cmds/GLCommands.h"
#include "../Scene/SceneNode.h"
#include "../Glm/glm.hpp"

#include "../Lighting/DirectionalLight.h"
#include "../Lighting/PointLight.h"
#include "CommandBuffer.h"
#include "RenderCommand.h"
#include "RenderTarget.h"

#include "../Mesh/Quad.h"
#include "../Mesh/Sphere.h"
#include "../Mesh/Cube.h"
#include "../Mesh/Mesh.h"

#include "Buffers.h"

namespace Exp
{
	class Mesh;
	class Material;
	class Camera;
	class Skybox;

	struct ProjectionUBO
	{
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewMatrix;
		glm::vec3 m_viewPosition;
	};

	class RenderingModule :
		public IModuleInterface
	{
	public:
		virtual ~RenderingModule();

		virtual void StartUp();
		virtual void Shutdown();
		virtual void DisplayUI();
	private:
		void InitGL();

		void UpdateGlobalUBO();
		void Render(RenderCommand* command, bool updateGLSettings=true);

		// Camera ProjectionUBO
		GLuint projUBOLocation;
		unsigned int projUBOId;
		ProjectionUBO projUBOData;


		// lighting
		std::vector<std::shared_ptr<DirectionalLight>> m_DirectionalLights;
		std::vector<std::shared_ptr<PointLight>> m_PointLights;
		std::shared_ptr<RenderTarget> m_GBuffer;
		std::shared_ptr<RenderTarget> m_CustomTarget;
		std::shared_ptr<Quad> m_NDCPlane;
		std::shared_ptr<Mesh> m_PointLightSphere;

		// Debug Lighting
		std::shared_ptr<Cube> m_DebugLightMesh;

		// Skybox
		std::shared_ptr<Skybox> CurrentSkybox;
	private:
		//Debug
		bool m_Wireframe = false;
		bool m_DisplayLights = true;
		bool m_DisplaySkybox = false;

		// 
		glm::vec2 m_RenderSize = glm::vec2(1280, 768);

	public:
		std::shared_ptr<RenderTarget> GetGBuffer();
		DirectionalLight * AddDirectionalLight(glm::vec3 Direction);
		PointLight * AddPointLight(glm::vec3 Position = glm::vec3(0.0f), float Radius = 1.0f);

		void SetCamera(Camera* Camera);

		void SetSkybox(std::string folder);
		void RenderSkybox();

		void PushMesh(SceneNode * Node);
		void Render();

		void ResizeRenderer(int Width, int Height);
	protected:
		void PushMeshRenderCommand(Mesh * mesh, Material * material, const glm::mat4 & transform);

		void RenderMesh(Mesh* mesh);

		void Blit(Texture* src, RenderTarget* dst = nullptr, Material* material=nullptr, std::string textureUniformName = "screenTexture");

		void RenderDeferredDirLight(DirectionalLight* light);
		void PreRenderDeferredPointLight(PointLight* light);
		void RenderDeferredPointLight(PointLight* light);

		void RenderDebugLights();

	public:
		Camera* RenderCamera;
		CommandBuffer m_CommandBuffer;
	};
}