#include "ECSModule.h"

#include <spdlog/spdlog.h>
#include "imgui.h"

#include "../module/ModuleManager.h"
#include "../rendering/RenderingModule.h"
#include "../scene/SceneNode.h"

namespace Exp
{
	void ECSModule::StartUp()
	{
		spdlog::info("ECSModule StartUp");
	}

	void ECSModule::PostInitialize()
	{
		renderingModule = ModuleManager::Get().GetModule<RenderingModule>("Rendering");
	}

	void ECSModule::Shutdown()
	{
		registry.clear();
		spdlog::info("ECSModule Shutdown");
	}

	entt::entity ECSModule::CreateEntity(const std::string& name, const TransformComponent& transform)
	{
		const entt::entity entity = registry.create();
		registry.emplace<NameComponent>(entity, name);
		registry.emplace<TransformComponent>(entity, transform);
		return entity;
	}

	void ECSModule::FixedUpdate(float fixedDeltaTime)
	{
		registry.view<TransformComponent, SpinComponent>().each(
			[fixedDeltaTime](auto entity, TransformComponent& transform, const SpinComponent& spin)
			{
				transform.rotation = glm::mod(transform.rotation + spin.degreesPerSecond * fixedDeltaTime, glm::vec3(360.0f));
			});
	}

	void ECSModule::SubmitRenderables()
	{
		if (!renderingModule)
			return;

		registry.view<TransformComponent, MeshRendererComponent>().each(
			[this](auto entity, const TransformComponent& transform, const MeshRendererComponent& renderer)
			{
				if (renderer.visible && renderer.model)
				{
					renderingModule->PushMesh(renderer.model, transform.Matrix(), renderer.castShadows);
				}
			});
	}

	void ECSModule::DisplayUI()
	{
		if (!ImGui::CollapsingHeader("Entities"))
			return;

		if (ImGui::Button("New entity"))
		{
			selected = CreateEntity("Entity");
		}

		ImGui::BeginChild("EntityList", ImVec2(0, 150), true);
		registry.each([this](entt::entity entity)
		{
			const NameComponent* name = registry.try_get<NameComponent>(entity);
			const std::string label = (name ? name->value : std::string("Entity")) + "##" + std::to_string(entt::to_integral(entity));
			if (ImGui::Selectable(label.c_str(), selected == entity))
			{
				selected = entity;
			}
		});
		ImGui::EndChild();

		if (selected != entt::null && registry.valid(selected))
		{
			DisplayInspector(selected);
		}
	}

	void ECSModule::DisplayInspector(entt::entity entity)
	{
		if (NameComponent* name = registry.try_get<NameComponent>(entity))
		{
			char buffer[128];
			snprintf(buffer, sizeof(buffer), "%s", name->value.c_str());
			if (ImGui::InputText("Name", buffer, sizeof(buffer)))
				name->value = buffer;
		}

		if (TransformComponent* transform = registry.try_get<TransformComponent>(entity))
		{
			ImGui::Separator();
			ImGui::TextUnformatted("Transform");
			ImGui::DragFloat3("Position", &transform->position[0], 0.1f);
			ImGui::DragFloat3("Rotation", &transform->rotation[0], 1.0f);
			ImGui::DragFloat3("Scale", &transform->scale[0], 0.05f, 0.001f, 1000.0f);
		}

		if (MeshRendererComponent* renderer = registry.try_get<MeshRendererComponent>(entity))
		{
			ImGui::Separator();
			ImGui::TextUnformatted("Mesh Renderer");
			ImGui::Checkbox("Visible", &renderer->visible);
			ImGui::Checkbox("Cast shadows", &renderer->castShadows);
		}

		if (SpinComponent* spin = registry.try_get<SpinComponent>(entity))
		{
			ImGui::Separator();
			ImGui::TextUnformatted("Spin");
			ImGui::DragFloat3("Degrees/s", &spin->degreesPerSecond[0], 1.0f);
			if (ImGui::Button("Remove Spin"))
				registry.remove<SpinComponent>(entity);
		}
		else if (ImGui::Button("Add Spin"))
		{
			registry.emplace<SpinComponent>(entity);
		}

		ImGui::Separator();
		if (ImGui::Button("Destroy entity"))
		{
			registry.destroy(entity);
			selected = entt::null;
		}
	}
}
