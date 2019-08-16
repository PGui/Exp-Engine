#pragma once

#include "../Module/ModuleInterface.h"
#include "../CommandBuffer/cmds/GLCommands.h"
#include "../Scene/SceneNode.h"
#include "../Glm/glm.hpp"

#include "Buffers.h"

namespace Exp
{
	class Mesh;
	class Material;
	class Camera;

	struct ProjectionUBO
	{
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewMatrix;
	};

	class RenderingModule :
		public IModuleInterface
	{
	public:
		RenderingModule();
		virtual ~RenderingModule();

		virtual void StartUp();
		virtual void Shutdown();
	private:
		void InitGL();

		void UpdateGlobalUBO();

		// Camera ProjectionUBO
		GLuint projUBOLocation;
		unsigned int projUBOId;
		ProjectionUBO projUBOData;

		

	public:
		void SetCamera(Camera* Camera);

		void PushMesh(SceneNode * Node);
		void Render();
	protected:
		void PushMeshRenderCommand(Mesh * mesh, Material * material, const glm::mat4 & transform);

	public:
		Camera* RenderCamera;
		GeometryCommandBuffer m_geometryCommands;
	};
}