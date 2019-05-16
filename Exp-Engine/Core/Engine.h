#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <ftl/task_scheduler.h>
#include <ftl/atomic_counter.h>
#include <ftl/task.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include "../camera/Camera.h"
#include "../Threading/Semaphore.h"
#include "../Threading/SpscQueue.h"

//#include "../rendering/renderer.h"
//#include "../scene/Scene.h"
#include <Remotery/Remotery.h>

namespace Exp
{
	struct GameSyncState
	{
		GameSyncState()
			: shouldQuit(false),
			  syncQueue(4) {}

		//Exp::Semaphore syncSempahore;
		Exp::SPSCQueue syncQueue;
		std::atomic_bool shouldQuit;
	};

	struct ThreadArgs
	{
		GLFWwindow * mainWindow;
		GLFWwindow * slaveWindow;
		GameSyncState * syncState;
	};

	class Engine
	{
	public:
		static unsigned int m_screenWidth;// = 1280;
		static unsigned int m_screenHeight;// = 768;

		static GameSyncState *syncState;

		static GLFWwindow*  m_mainWindow;// = nullptr;
		static GLFWwindow*  m_slaveWindow; // = nullptr;

		static double m_deltaTime;// = 1.0 / 60.0;
		static double m_lastTime;// = 0.0;
		static double m_accumulatedTime;// = 0.0;
		static const double m_updatePeriod;// = 1/60;
		static const double m_precisionUpdatePeriod;
		static const int m_maxUpdatesPerLoop;

		//Camera
		static Camera * m_Camera;// = nullptr;

		//Debug render parameters
		static bool m_wireframe; //= false;
		static bool m_debugLight; //=false;

		//Renderer
		//static Renderer * EngineRenderer;

		static void MainJob(ftl::TaskScheduler *taskScheduler, void *arg);

		static GLFWwindow * InitWindow(std::string title, bool fullScreen = false, GLFWwindow* shared = nullptr, bool visible = true);

		static void ParallelJob(ftl::TaskScheduler *taskScheduler, void *arg) 
		{
			int i = *reinterpret_cast<int*>(arg);
			rmt_ScopedCPUSample(ParallelJob, 0);
			//std::cout << "Job " << i << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(4));
		}


		//Callbacks
		static void framebuffer_size_callback(GLFWwindow * window, int width, int height);
		static void glfw_error_callback(int error, const char* description);


		//Renderer Initialization
		//static Renderer * Init()
		//{
		//	glfwInit();
		//	const char* glsl_version = "#version 330";
		//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		//	g_Window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "opengl-exp", nullptr, nullptr);
		//	if (g_Window == nullptr)
		//	{
		//		std::cout << "Failed to initialize GLFW window." << std::endl;
		//		glfwTerminate();
		//		return nullptr;
		//	}

		//	glfwMakeContextCurrent(g_Window);

		//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		//	{
		//		std::cout << "Failed to initialize glad." << std::endl;
		//		glfwTerminate();
		//		return nullptr;
		//	}

		//	glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		//	glfwSetErrorCallback(Engine::glfw_error_callback);
		//	glfwSetFramebufferSizeCallback(g_Window, Engine::framebuffer_size_callback);

		//	InitImgui(glsl_version);

		//	EngineRenderer = new Renderer();

		//	EngineRenderer->m_Camera = &EngineCamera;

		//	int nrAttributes;
		//	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
		//	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;
		//	//glEnable(GL_DEPTH_TEST);

		//	return EngineRenderer;
		//}

		//static void Cleanup()
		//{
		//	Engine::CleanupImgui();

		//	Scene::Clear();

		//	delete EngineRenderer;
		//}

		////Time update
		//static void Update()
		//{
		//	float currentFrame = (float)glfwGetTime();
		//	deltaTime = currentFrame - lastFrame;
		//	lastFrame = currentFrame;

		//	EngineRenderer->m_Camera->update(deltaTime);
		//}

		////GLFW Callbacks
		//static void glfw_error_callback(int error, const char* description)
		//{
		//	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
		//}

		//static void framebuffer_size_callback(GLFWwindow * window, int width, int height)
		//{
		//	glViewport(0, 0, width, height);
		//	EngineRenderer->m_Camera->setPerspective(EngineRenderer->m_Camera->_fovY, (float)width / (float)height, EngineRenderer->m_Camera->_nearPlane, EngineRenderer->m_Camera->_farPlane);
		//}

		//static void ProcessInput()
		//{
		//	if (glfwGetKey(g_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		//	{
		//		glfwSetWindowShouldClose(g_Window, true);
		//	}

		//	if (glfwGetKey(g_Window, GLFW_KEY_W) == GLFW_PRESS)
		//		EngineRenderer->m_Camera->updateKey(deltaTime, CameraMovement::FORWARD);
		//	if (glfwGetKey(g_Window, GLFW_KEY_S) == GLFW_PRESS)
		//		EngineRenderer->m_Camera->updateKey(deltaTime, CameraMovement::BACKWARD);
		//	if (glfwGetKey(g_Window, GLFW_KEY_A) == GLFW_PRESS)
		//		EngineRenderer->m_Camera->updateKey(deltaTime, CameraMovement::LEFT);
		//	if (glfwGetKey(g_Window, GLFW_KEY_D) == GLFW_PRESS)
		//		EngineRenderer->m_Camera->updateKey(deltaTime, CameraMovement::RIGHT);
		//	if (glfwGetKey(g_Window, GLFW_KEY_SPACE) == GLFW_PRESS)
		//		EngineRenderer->m_Camera->updateKey(deltaTime, CameraMovement::UP);
		//	if (glfwGetKey(g_Window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		//		EngineRenderer->m_Camera->updateKey(deltaTime, CameraMovement::DOWN);
		//}

		//static void MouseInput(double xpos, double ypos)
		//{
		//	static bool firstMouse = true;
		//	static double lastX = 0.0;
		//	static double lastY = 0.0;
		//	if (firstMouse) // this bool variable is initially set to true
		//	{
		//		lastX = xpos;
		//		lastY = ypos;
		//		firstMouse = false;
		//	}

		//	EngineRenderer->m_Camera->updateMouse((float)(xpos - lastX), (float)(lastY - ypos));

		//	lastX = xpos;
		//	lastY = ypos;
		//}

		//static void KeyInput(int key, int scancode, int action, int mods)
		//{
		//	if (key == GLFW_KEY_F1 && action == GLFW_RELEASE)
		//	{
		//		EngineRenderer->m_Camera->_disableMouse = !EngineRenderer->m_Camera->_disableMouse;
		//		if (EngineRenderer->m_Camera->_disableMouse)
		//		{
		//			glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		//		}
		//		else
		//		{
		//			glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		//		}
		//	}
		//}

		////IMGUI
		//static void InitImgui(const char* glsl_version)
		//{
		//	// Setup Dear ImGui context
		//	IMGUI_CHECKVERSION();
		//	ImGui::CreateContext();
		//	ImGuiIO& io = ImGui::GetIO(); (void)io;

		//	// Setup Dear ImGui style
		//	ImGui::StyleColorsDark();

		//	// Setup Platform/Renderer bindings
		//	ImGui_ImplGlfw_InitForOpenGL(g_Window, true);
		//	ImGui_ImplOpenGL3_Init(glsl_version);
		//}

		//static void CleanupImgui()
		//{
		//	// Cleanup
		//	ImGui_ImplOpenGL3_Shutdown();
		//	ImGui_ImplGlfw_Shutdown();
		//	ImGui::DestroyContext();
		//}

		//static void NewFrame()
		//{
		//	ImGui_ImplOpenGL3_NewFrame();
		//	ImGui_ImplGlfw_NewFrame();
		//	ImGui::NewFrame();
		//}

		//static void DisplayUI()
		//{
		//	ImGui::Begin("opengl-exp");
		//	ImGui::Checkbox("Wireframe", &bWireframe);
		//	ImGui::Text("Debug Render Mode");
		//	ImGui::RadioButton("None", &currentDebugRenderMode, eDebugRenderMode::None);
		//	ImGui::RadioButton("Unlit", &currentDebugRenderMode, eDebugRenderMode::Unlit);
		//	ImGui::RadioButton("Depth", &currentDebugRenderMode, eDebugRenderMode::Depth);

		//	ImGui::Checkbox("Debug Lights", &bDebugLight);
		//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//	for (int i = 0; i < EngineRenderer->m_PointLights.size(); ++i)
		//	{
		//		ImGui::ColorEdit3(std::string("PointLight" + std::to_string(i)).c_str(), &Engine::EngineRenderer->m_PointLights[i]->Color[0]);
		//	}

		//	ImGui::End();

		//	if (bWireframe)
		//	{
		//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//	}
		//	else
		//	{
		//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//	}
		//}


	};
}
