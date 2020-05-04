#pragma once

//#include <ftl/task_scheduler.h>
//#include <ftl/atomic_counter.h>
//#include <ftl/task.h>

#include <glad/glad.h>

#include <iostream>
#include <spdlog/spdlog.h>

#include "../Module/ModuleInterface.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../Camera/Camera.h"
#include <GLFW/glfw3.h>

namespace Exp
{
	class InputModule;
	class RenderingModule;
	class MaterialLibraryModule;

	class EngineModule:
		public IModuleInterface
	{
	public:
		//ftl::TaskScheduler * taskScheduler = nullptr;
	public:


		GLFWwindow*  mainWindow = nullptr;

		// Deltatime
		double deltaTime = 1.0 / 60.0;
		double lastTime = 0.0;
		double accumulatedTime = 0.0;
		const double updatePeriod = 1.0/60.0;
		//const double m_precisionUpdatePeriod ;
		const int maxUpdatesPerLoop = 1;

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

	public:
		//IModuleInterface
		virtual void StartUp();
		virtual void Shutdown();
		virtual void DisplayUI();
		virtual void PostInitialize();

		// Methods
		GLFWwindow * InitWindow(std::string title, bool fullScreen = false, GLFWwindow* shared = nullptr, bool visible = true);

		//Main loop
		void RunEngine();

		void NewFrame();
		void Update();
		void EndFrame();

		// Delta time
		void ComputeDeltatime();

	public:

		//Callbacks
		static void FramebufferSizeCallback(GLFWwindow * window, int width, int height);
		static void GlfwErrorCallback(int error, const char* description);
	};
}