#pragma once
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "module/ModuleInterface.h"
#include "scene/SceneNode.h"

#include "CommandBuffer.h"
#include "RenderCommand.h"
#include "RenderTarget.h"
#include "GLCache.h"

#include "lighting/DirectionalLight.h"
#include "lighting/PointLight.h"

#include "mesh/Quad.h"
#include "mesh/Sphere.h"
#include "mesh/Cube.h"
#include "mesh/Mesh.h"

namespace Exp
{
	class Mesh;
	class Material;
	class Camera;
	class Skybox;
	class PostProcessor;

	struct ProjectionUBO
	{
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;
		glm::vec3 viewPosition;
	};

	class RenderingModule :
		public IModuleInterface
	{
		friend class PostProcessor;
	public:
		virtual ~RenderingModule();

		virtual void StartUp();
		virtual void Shutdown();
		virtual void DisplayUI();
		virtual void PostInitialize();
	private:
		//Modules
		class MaterialLibraryModule* materialLibrary = nullptr;
		class ECSModule* ecsModule = nullptr;
	private:
		void InitGL();

		void UpdateGlobalUBO();
		void Render(RenderCommand* command, bool updateGLSettings=true);

		// Post Process
		std::shared_ptr<PostProcessor> postProcessor;

		// Camera ProjectionUBO
		GLuint projUBOLocation;
		unsigned int projUBOId;
		ProjectionUBO projUBOData;


		// lighting
		std::vector<std::shared_ptr<DirectionalLight>> directionalLights;
		std::vector<std::shared_ptr<PointLight>> pointLights;
		std::shared_ptr<RenderTarget> GBuffer;
		std::shared_ptr<RenderTarget> renderingTarget;
		std::shared_ptr<Quad> ndcPlane;
		std::shared_ptr<Mesh> pointLightSphere;

		// Debug Lighting
		std::shared_ptr<Cube> debugLightMesh;

		// Skybox
		std::shared_ptr<Skybox> currentSkybox;
	private:
		//Debug
		bool wireframe = false;
		bool displayLights = true;
		bool displaySkybox = false;

		//
		glm::tvec2<GLint> renderSize = glm::vec2(1280, 768);

	public:
		std::shared_ptr<RenderTarget> GetGBuffer();
		DirectionalLight * AddDirectionalLight(glm::vec3 direction);
		PointLight * AddPointLight(glm::vec3 position = glm::vec3(0.0f), float radius = 1.0f);

		void SetCamera(Camera* camera);

		void SetSkybox(std::string folder);
		void RenderSkybox();

		void PushMesh(SceneNode * node);
		void Render();

		void ResizeRenderer(int width, int height);
	protected:
		void PushMeshRenderCommand(Mesh * mesh, Material * material, const glm::mat4 & transform);

		void RenderMesh(Mesh* mesh);

		void Blit(Texture* src, RenderTarget* dst = nullptr, Material* material=nullptr, std::string textureUniformName = "screenTexture");

		void RenderDeferredDirLight(DirectionalLight* light);
		void PreRenderDeferredPointLight(PointLight* light);
		void RenderDeferredPointLight(PointLight* light);

		void RenderDebugLights();

	public:
		Camera* renderCamera;
		CommandBuffer commandBuffer;
	};
}