#pragma once

#include <ftl/task_scheduler.h>
#include <ftl/atomic_counter.h>
#include <ftl/task.h>

#include <glad/glad.h>

#include <iostream>
#include <spdlog/spdlog.h>

#include "../Module/ModuleInterface.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

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
		ftl::TaskScheduler * taskScheduler = nullptr;
	public:
		

		GLFWwindow*  m_mainWindow = nullptr;

		// Deltatime
		double m_deltaTime = 1.0 / 60.0;
		double m_lastTime = 0.0;
		double m_accumulatedTime = 0.0;
		const double m_updatePeriod = 1.0/60.0;
		//const double m_precisionUpdatePeriod ;
		const int m_maxUpdatesPerLoop = 1;

		//Camera
		Camera m_Camera = Camera(glm::vec3(0.0f));

		//Debug render parameters
		bool m_wireframe = false;
		bool m_debugLight = false;

	// Modules
	public:
		InputModule * m_inputModule = nullptr;
		RenderingModule * m_renderingModule = nullptr;
		MaterialLibraryModule * m_materialLibraryModule = nullptr;

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
		static void framebuffer_size_callback(GLFWwindow * window, int width, int height);
		static void glfw_error_callback(int error, const char* description);
	};
}