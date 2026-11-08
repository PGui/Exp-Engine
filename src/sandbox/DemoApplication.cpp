#include "DemoApplication.h"

#include <filesystem>

#include "helpers/RootDir.h"
#include "core/EngineModule.h"
#include "ecs/ECSModule.h"
#include "rendering/RenderingModule.h"
#include "materialLibrary/MaterialLibraryModule.h"
#include "resources/Resources.h"
#include "scene/Scene.h"
#include "mesh/Cube.h"
#include "mesh/Sphere.h"
#include "mesh/Plane.h"
#include "lighting/DirectionalLight.h"
#include "lighting/PointLight.h"

namespace Exp
{
	void DemoApplication::OnStart(EngineModule& engine)
	{
		ECSModule& ecs = *engine.ecsModule;
		MaterialLibraryModule& materials = *engine.materialLibraryModule;
		RenderingModule& renderer = *engine.renderingModule;

		auto addRenderable = [&](const char* name, SceneNode* model, const TransformComponent& transform)
		{
			const entt::entity entity = ecs.CreateEntity(name, transform);
			ecs.registry.emplace<MeshRendererComponent>(entity, model);
			return entity;
		};

		// Floor: the Plane mesh lies in XY, facing -Z; rotate it to face +Y.
		{
			auto plane = std::make_unique<Plane>(10, 10);
			Material* floorMaterial = materials.CreateMaterial("default");
			floorMaterial->SetTexture("material.diffuse",
				Resources::LoadTexture("wood", ROOT_DIR "resources/texture/wood.png", GL_TEXTURE_2D, GL_RGB, true), 0); // color texture: sRGB
			SceneNode* node = Scene::MakeSceneNode(plane.get(), floorMaterial);
			ownedMeshes.push_back(std::move(plane));

			TransformComponent t;
			t.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
			t.scale = glm::vec3(30.0f, 30.0f, 1.0f);
			const entt::entity floor = addRenderable("Floor", node, t);
			ecs.registry.get<MeshRendererComponent>(floor).castShadows = false;
		}

		// Grid of spinning bunnies (one shared model, many entities).
		{
			SceneNode* bunny = Resources::LoadMesh(nullptr, "bunny", ROOT_DIR "resources/models/bunny/bunny.obj");
			for (int i = 0; i < 5; ++i)
			{
				for (int j = 0; j < 5; ++j)
				{
					TransformComponent t;
					t.position = glm::vec3(-10.0f + i * 5.0f, 0.35f, -10.0f + j * 5.0f);
					t.rotation.y = (i * 5 + j) * 25.0f;
					const entt::entity e = addRenderable("Bunny", bunny, t);
					ecs.registry.emplace<SpinComponent>(e, glm::vec3(0.0f, 20.0f + 8.0f * ((i + j) % 4), 0.0f));
				}
			}
		}

		// Floating spheres.
		{
			auto sphere = std::make_unique<Sphere>(32, 32);
			SceneNode* node = Scene::MakeSceneNode(sphere.get(), materials.GetMaterial("default"));
			ownedMeshes.push_back(std::move(sphere));
			for (int i = 0; i < 5; ++i)
			{
				TransformComponent t;
				t.position = glm::vec3(-10.0f + i * 5.0f, 5.0f, 12.0f);
				addRenderable("Sphere", node, t);
			}
		}

		// Spinning cube.
		{
			auto cube = std::make_unique<Cube>();
			SceneNode* node = Scene::MakeSceneNode(cube.get(), materials.GetMaterial("default"));
			ownedMeshes.push_back(std::move(cube));
			TransformComponent t;
			t.position = glm::vec3(14.0f, 1.5f, 0.0f);
			t.scale = glm::vec3(1.5f);
			const entt::entity e = addRenderable("Cube", node, t);
			ecs.registry.emplace<SpinComponent>(e, glm::vec3(15.0f, 40.0f, 0.0f));
		}

		// Tree (the model is ~90 units tall).
		{
			SceneNode* maple = Resources::LoadMesh(nullptr, "maple", ROOT_DIR "resources/models/maple/maple.obj");
			TransformComponent t;
			t.position = glm::vec3(-16.0f, 0.0f, -16.0f);
			t.scale = glm::vec3(0.12f);
			addRenderable("Maple", maple, t);
		}

		// Lights: a sun casting shadows and a coloured point light.
		sun = renderer.AddDirectionalLight(glm::normalize(glm::vec3(-0.5f, 1.0f, 0.35f)));
		sun->color = glm::vec3(1.0f, 0.95f, 0.85f);
		sun->intensity = 2.5f; // linear HDR lighting, tonemapped with ACES
		sun->castShadows = true;

		PointLight* point = renderer.AddPointLight(glm::vec3(3.0f, 3.0f, 4.0f), 14.0f);
		point->color = glm::vec3(0.4f, 0.6f, 1.0f);

		// The skybox textures are not versioned: only use them when present.
		const char* skyboxFolder = ROOT_DIR "resources/skyboxes/Fjaderholmarna/";
		if (std::filesystem::exists(skyboxFolder))
			renderer.SetSkybox(skyboxFolder);

		engine.camera.SetTransform(glm::vec3(-22.0f, 16.0f, 26.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void DemoApplication::OnUI(EngineModule& engine)
	{
		ImGui::Begin("Exp Engine");
		if (sun && ImGui::CollapsingHeader("Demo"))
		{
			// Animate the sun around the scene to show off the shadows.
			static float sunAngle = 35.0f;
			if (ImGui::SliderFloat("Sun angle", &sunAngle, 0.0f, 360.0f))
			{
				const float a = glm::radians(sunAngle);
				sun->direction = glm::normalize(glm::vec3(std::cos(a), 1.2f, std::sin(a)));
			}
		}
		ImGui::End();
	}

	void DemoApplication::OnShutdown(EngineModule& engine)
	{
		// Entities reference our meshes through scene nodes: drop them first,
		// and release GPU buffers while the GL context is still alive.
		engine.ecsModule->registry.clear();
		Scene::Clear();
		ownedMeshes.clear();
	}
}
