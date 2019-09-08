#pragma once
#include "../Module/ModuleInterface.h"
#include "../CommandBuffer/cmds/GLCommands.h"
#include "../Scene/SceneNode.h"
#include "../Glm/glm.hpp"

#include "../Lighting/DirectionalLight.h"
#include "CommandBuffer.h"
#include "RenderCommand.h"
#include "RenderTarget.h"

#include "../Mesh/Quad.h"

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
		std::shared_ptr<RenderTarget> m_GBuffer;
		std::shared_ptr<RenderTarget> m_CustomTarget;
		std::shared_ptr<Quad> m_NDCPlane;

		// Skybox
		std::shared_ptr<Skybox> CurrentSkybox;
	private:
		//Debug
		bool m_Wireframe = false;

	public:
		std::shared_ptr<RenderTarget> GetGBuffer();
		std::shared_ptr<DirectionalLight> AddDirectionalLight(glm::vec3 Direction);

		void SetCamera(Camera* Camera);

		void SetSkybox(std::string folder);
		void RenderSkybox();

		void PushMesh(SceneNode * Node);
		void Render();
	protected:
		void PushMeshRenderCommand(Mesh * mesh, Material * material, const glm::mat4 & transform);

		void RenderMesh(Mesh* mesh);

		void Blit(Texture* src, RenderTarget* dst = nullptr, Material* material=nullptr, std::string textureUniformName = "screenTexture");

		void RenderDeferredDirLight(DirectionalLight* light);

	public:
		Camera* RenderCamera;
		CommandBuffer m_CommandBuffer;
	};
}