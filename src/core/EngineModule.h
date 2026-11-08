#pragma once

//#include <ftl/task_scheduler.h>
//#include <ftl/atomic_counter.h>
//#include <ftl/task.h>

#include <glad/glad.h>

#include <iostream>
#include <spdlog/spdlog.h>

#include "module/ModuleInterface.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../camera/Camera.h"
#include <GLFW/glfw3.h>

namespace Exp
{
	class InputModule;
	class RenderingModule;
	class MaterialLibraryModule;
	class ECSModule;
	class Application;

	class EngineModule:
		public IModuleInterface
	{
	public:
		//ftl::TaskScheduler * taskScheduler = nullptr;
	public:


		GLFWwindow*  mainWindow = nullptr;

		// Timing
		enum class SimulationMode
		{
			FixedInterpolated, // simulation at a fixed rate, rendering interpolates between steps
			Variable           // simulation runs once per frame with the frame delta time
		};
		SimulationMode simulationMode = SimulationMode::FixedInterpolated;

		double deltaTime = 1.0 / 60.0;          // real frame time (variable)
		double lastTime = 0.0;
		double accumulatedTime = 0.0;
		double simulationTime = 0.0;            // total simulated time
		double updatePeriod = 1.0 / 60.0;       // fixed simulation step (FixedInterpolated mode)
		int maxUpdatesPerLoop = 8;              // max fixed steps per frame
		double maxDeltaTime = 0.1;              // clamp for long frames (debugger, window drag...)
		float interpolationAlpha = 1.0f;        // [0,1] position between the last two fixed steps

		bool vsync = true;
		int maxFps = 0;                         // 0 = uncapped (only applies when vsync is off)

		// Frame time history for the UI graph (ms).
		static constexpr int frameHistorySize = 240;
		float frameTimes[frameHistorySize] = {};
		int frameTimeIndex = 0;

		void SetVSync(bool enabled);

		//Camera
		Camera camera = Camera(glm::vec3(0.0f));

		//Debug render parameters
		bool wireframe = false;
		bool debugLight = false;

	// Modules
	public:
		InputModule * inputModule = nullptr;
		RenderingModule * renderingModule = nullptr;
		MaterialLibraryModule * materialLibraryModule = nullptr;
		ECSModule* ecsModule = nullptr;
	private:
		Application* application = nullptr;
	public:
		~EngineModule() override;

		//IModuleInterface
		virtual void StartUp();
		virtual void Shutdown();
		virtual void DisplayUI();
		virtual void PostInitialize();

		// Methods
		GLFWwindow * InitWindow(std::string title, bool fullScreen = false, GLFWwindow* shared = nullptr, bool visible = true);

		//Main loop. maxFrames <= 0 runs until the window is closed.
		void RunEngine(Application& app, int maxFrames = 0);

		void NewFrame();
		void Update();
		void EndFrame();

		// Delta time
		void ComputeDeltatime();
		void LimitFrameRate(double frameStartTime);

	public:

		//Callbacks
		static void FramebufferSizeCallback(GLFWwindow * window, int width, int height);
		static void GlfwErrorCallback(int error, const char* description);
		static void APIENTRY GlDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
	};
}