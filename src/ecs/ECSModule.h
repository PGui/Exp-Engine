#pragma once
#include <string>

#include <entt/entt.hpp>

#include "module/ModuleInterface.h"
#include "Components.h"

namespace Exp
{
	class RenderingModule;

	class ECSModule :
		public IModuleInterface
	{
	public:
		entt::registry registry;

	public:
		virtual void StartUp() override;
		virtual void Shutdown() override;
		virtual void DisplayUI() override;
		virtual void PostInitialize() override;

		// Creates an entity with a name and a transform.
		entt::entity CreateEntity(const std::string& name, const TransformComponent& transform = {});

		// Gameplay systems, run at the fixed simulation rate.
		void FixedUpdate(float fixedDeltaTime);

		// Pushes every visible MeshRenderer to the renderer for this frame.
		void SubmitRenderables();

	private:
		RenderingModule* renderingModule = nullptr;
		entt::entity selected = entt::null;

		void DisplayInspector(entt::entity entity);
	};
}
