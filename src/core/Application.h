#pragma once

namespace Exp
{
	class EngineModule;

	/*
		Game / sandbox code plugs into the engine through this interface, so the engine
		core no longer contains any scene-specific code.

		Frame order:
			OnUpdate (variable dt) -> OnFixedUpdate (0..N times, fixed dt) -> ECS submit -> Render
	*/
	class Application
	{
	public:
		virtual ~Application() = default;

		// Called once, after every module is initialized and before the first frame.
		virtual void OnStart(EngineModule& engine) {}

		// Called once per rendered frame with the real elapsed time.
		virtual void OnUpdate(EngineModule& engine, float deltaTime) {}

		// Called at a fixed rate (EngineModule::updatePeriod); use it for simulation.
		virtual void OnFixedUpdate(EngineModule& engine, float fixedDeltaTime) {}

		// Called once per frame, inside the ImGui frame, to add app-specific UI.
		virtual void OnUI(EngineModule& engine) {}

		// Called before modules shut down.
		virtual void OnShutdown(EngineModule& engine) {}
	};
}
