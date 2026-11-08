#pragma once
#include <memory>
#include <vector>

#include "core/Application.h"

namespace Exp
{
	class Mesh;
	class DirectionalLight;

	/*
		Sandbox scene used to exercise the engine: a floor, a grid of spinning bunnies,
		a few spheres, a tree, a sun (with shadows) and a point light.
		Everything is built from ECS entities.
	*/
	class DemoApplication : public Application
	{
	public:
		void OnStart(EngineModule& engine) override;
		void OnUI(EngineModule& engine) override;
		void OnShutdown(EngineModule& engine) override;

	private:
		// Procedural meshes are owned by the application (model files are owned by Resources).
		std::vector<std::unique_ptr<Mesh>> ownedMeshes;
		DirectionalLight* sun = nullptr;
	};
}
