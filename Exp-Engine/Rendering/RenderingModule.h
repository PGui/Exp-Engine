#pragma once

#include "../Module/ModuleInterface.h"
#include "../CommandBuffer/cmds/GLCommands.h"
#include "../Scene/SceneNode.h"
#include "../Glm/glm.hpp"
#include "CommandBuffer.h"
#include "RenderCommand.h"

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

		

	public:
		void SetCamera(Camera* Camera);

		void PushMesh(SceneNode * Node);
		void Render();
	protected:
		void PushMeshRenderCommand(Mesh * mesh, Material * material, const glm::mat4 & transform);

		void RenderMesh(Mesh* mesh);

	public:
		Camera* RenderCamera;
		CommandBuffer m_CommandBuffer;
	};
}