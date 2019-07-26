#pragma once
#include "Buffers.h"

#include "../Module/ModuleInterface.h"
#include "../CommandBuffer/cmds/GLCommands.h"
#include "../Scene/SceneNode.h"
#include "../Glm/glm.hpp"

namespace Exp
{
	class Mesh;
	class Material;

	class RenderingModule :
		public IModuleInterface
	{
	public:
		RenderingModule();
		virtual ~RenderingModule();

		virtual void StartUp();
		virtual void Shutdown();

	public:
		void PushMesh(SceneNode * Node);

		void Render();
	protected:
		void PushMeshRenderCommand(Mesh * mesh, Material * material, const glm::mat4 & transform);

	public:
		GeometryCommandBuffer m_geometryCommands;
	};
}