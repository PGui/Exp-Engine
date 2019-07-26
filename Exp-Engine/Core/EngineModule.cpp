#include "EngineModule.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include <memory_lib/tagged_heap.h>
#include "../Threading/Semaphore.h"

#include <Remotery/Remotery.h>

#include "../Module/ModuleManager.h"
#include "../Rendering/RenderingModule.h"
#include "../Input/InputModule.h"
#include "../MaterialLibrary/MaterialLibraryModule.h"

#include "../Scene/SceneNode.h"

GLFWwindow * Exp::EngineModule::InitWindow(std::string title, bool fullScreen, GLFWwindow* shared, bool visible)
{
	GLFWwindow* win;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (visible)
		glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
	else
		glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

	GLFWmonitor* monitor = nullptr;

	if (fullScreen)
	{
		if (visible) //Don't create fullscreen window for offscreen contexts
			monitor = glfwGetPrimaryMonitor();
	}
	
	win = glfwCreateWindow(m_screenWidth, m_screenHeight, title.c_str(), monitor, shared);
	
	return win;
}
void Exp::EngineModule::RunEngine()
{
	m_inputModule = ModuleManager::Get().GetModule<InputModule>("Input");
	m_renderingModule = ModuleManager::Get().GetModule<RenderingModule>("Rendering");
	m_materialLibraryModule = ModuleManager::Get().GetModule<MaterialLibraryModule>("MaterialLibrary");

	//TEST
	SceneNode * Bunny = Resources::LoadMesh(nullptr, "bunny", "../resources/models/bunny/bunny.obj");

	while (!glfwWindowShouldClose(m_mainWindow))
	{
		rmt_ScopedCPUSample(LogicLoop, 0);

		rmt_BeginCPUSample(PollEvents, 0);
		// Check for events
		glfwPollEvents();
		rmt_EndCPUSample();

		NewFrame();

		Update();

		static glm::vec4 clear_color = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
		{
			static float f = 0.0f;
			static int counter = 0;
			
			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();

			{
				const auto key = cb::DrawKey::makeDefault(0, cb::ViewLayerType::e3D);
				auto* cmd = m_renderingModule->m_geometryCommands.addCommand<cmds::ClearColor>(key);
				cmd->red = clear_color.r;
				cmd->green = clear_color.g;
				cmd->blue = clear_color.b;
				cmd->alpha = clear_color.a;
				cmd->flags = GL_COLOR_BUFFER_BIT;

				CB_DEBUG_COMMAND_TAG(cmd);
				CB_DEBUG_COMMAND_TAG_MSG(cmd, "ClearColor");
				CB_DEBUG_COMMAND_SET_MSG(cmd, "ClearColor");
			}
		}

		m_renderingModule->PushMesh(Bunny);

		//Rendering
		m_renderingModule->Render();

		EndFrame();
	}
}

void Exp::EngineModule::NewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Exp::EngineModule::Update()
{
	rmt_ScopedCPUSample(MainThreadUpdate, 0);

	ComputeDeltatime();

	int updates = 0;
	while (m_accumulatedTime >= m_updatePeriod)
	{
		m_accumulatedTime -= m_updatePeriod;
		if (m_accumulatedTime < (1.0 / 59.0) - m_updatePeriod)
		{
			m_accumulatedTime = 0;
		}
		{
			m_inputModule->Update((float)m_deltaTime);

			{
				rmt_ScopedCPUSample(CameraUpdate, 0);

				if (m_inputModule->IsKeyPressed(GLFW_KEY_W))
					m_Camera.UpdateKey((float)m_deltaTime, CameraMovement::FORWARD);
				if (m_inputModule->IsKeyPressed(GLFW_KEY_S))
					m_Camera.UpdateKey((float)m_deltaTime, CameraMovement::BACKWARD);
				if (m_inputModule->IsKeyPressed(GLFW_KEY_A))
					m_Camera.UpdateKey((float)m_deltaTime, CameraMovement::LEFT);
				if (m_inputModule->IsKeyPressed(GLFW_KEY_D))
					m_Camera.UpdateKey((float)m_deltaTime, CameraMovement::RIGHT);
				if (m_inputModule->IsKeyPressed(GLFW_KEY_SPACE))
					m_Camera.UpdateKey((float)m_deltaTime, CameraMovement::UP);
				if (m_inputModule->IsKeyPressed(GLFW_KEY_LEFT_CONTROL))
					m_Camera.UpdateKey((float)m_deltaTime, CameraMovement::DOWN);
				glm::vec2 mouseDelta = m_inputModule->GetMouseDelta();
				m_Camera.UpdateMouse(mouseDelta.x, mouseDelta.y);
				m_Camera.Update((float)m_deltaTime);
			}
			
			updates++;
		}
	}
}

void Exp::EngineModule::EndFrame()
{
	ImGui::EndFrame();
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(m_mainWindow);
}

void Exp::EngineModule::ComputeDeltatime()
{
	// Logic
	double currentTime = glfwGetTime();
	m_deltaTime = (currentTime - m_lastTime);/* / m_updatePeriod;*/
	m_lastTime = currentTime;

	if (abs(m_deltaTime - 1.0 / 120.0) < .0002) {
		m_deltaTime = 1.0 / 120.0;
	}
	if (abs(m_deltaTime - 1.0 / 60.0) < .0002) {
		m_deltaTime = 1.0 / 60.0;
	}
	if (abs(m_deltaTime - 1.0 / 30.0) < .0002) {
		m_deltaTime = 1.0 / 30.0;
	}

	m_accumulatedTime += m_deltaTime;

	// Avoid spiral of death
	// Drop world time from the update in order to keep up
	if (m_accumulatedTime >= m_updatePeriod * m_maxUpdatesPerLoop) {
		m_accumulatedTime = m_updatePeriod * m_maxUpdatesPerLoop;
	}
}

void Exp::EngineModule::framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	if (EngineModule * Core = ModuleManager::Get().GetModule<Exp::EngineModule>("Engine"))
	{
		Core->m_screenWidth = width;
		Core->m_screenHeight = height;
		glViewport(0, 0, Core->m_screenWidth, Core->m_screenHeight);

		Core->m_Camera.SetPerspective(
			Core->m_Camera.m_fovY, 
			(float)width / (float)height, 
			Core->m_Camera.m_nearPlane, 
			Core->m_Camera.m_farPlane);
	}
}
void Exp::EngineModule::glfw_error_callback(int error, const char * description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void Exp::EngineModule::StartUp()
{
	rmt_ScopedCPUSample(EngineModuleStartUp, 0);

	if (!glfwInit())
	{
		std::cout << "Failed to initialize glfw" << std::endl;
		return;
	}

	//main window
	m_mainWindow = InitWindow("Exp-Engine");

	if (!Exp::EngineModule::m_mainWindow )
	{
		glfwTerminate();
		std::cout << "Failed to create glfw windows." << std::endl;
		return;
	}

	glfwMakeContextCurrent(m_mainWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize glad." << std::endl;
		glfwTerminate();
		return;
	}

	glfwSetInputMode(m_mainWindow, GLFW_CURSOR, GLFW_CURSOR);

	glfwSetFramebufferSizeCallback(m_mainWindow, framebuffer_size_callback);

	// Setup Dear ImGui context
	const char* glsl_version = "#version 330";
	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(m_mainWindow, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	std::cout << "EngineModule StartUp" << std::endl;
}

void Exp::EngineModule::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_mainWindow);

	std::cout << "EngineModule Shutdown" << std::endl;
}



//void Exp::Engine::MainJob(ftl::TaskScheduler * taskScheduler, void * arg)
//{
	////Init Cam
	//Exp::Engine::m_Camera(glm::vec3(0.0f));

	//uint64 sceneNumber = 1;

	//glfwMakeContextCurrent(nullptr);

	//size_t numTasks = 6;
	//ftl::Task *tasks = new ftl::Task[numTasks];
	//int *numbers = new int[numTasks];
	//for (uint64 i = 0ull; i < numTasks; ++i) 
	//{
	//	numbers[i] = (int)i;
	//	tasks[i] = { Exp::Engine::ParallelJob , &numbers[i] };
	//}

	//m_lastTime = glfwGetTime();

	//syncState->syncMTSempahore.Signal(); //Don't make the MT Wait first frame.

	//bool firstFrame = true;

	//// Loop until there is a quit message from the window or the user.
	//while (!glfwWindowShouldClose(Exp::Engine::m_mainWindow))
	//{
	//	rmt_ScopedCPUSample(LogicLoop, 0);

	//	rmt_BeginCPUSample(PollEvents, 0);
	//	// Check for events
	//	glfwPollEvents();
	//	rmt_EndCPUSample();

	//	// Logic
	//	double currentTime = glfwGetTime();
	//	m_deltaTime = (currentTime - m_lastTime);/* / m_updatePeriod;*/
	//	m_lastTime = currentTime;

	//	if (abs(m_deltaTime - 1.0 / 120.0) < .0002) {
	//		m_deltaTime = 1.0 / 120.0;
	//	}
	//	if (abs(m_deltaTime - 1.0 / 60.0) < .0002) {
	//		m_deltaTime = 1.0 / 60.0;
	//	}
	//	if (abs(m_deltaTime - 1.0 / 30.0) < .0002) {
	//		m_deltaTime = 1.0 / 30.0;
	//	}

	//	m_accumulatedTime += m_deltaTime;

	//	// Avoid spiral of death
	//	// Drop world time from the update in order to keep up
	//	if (m_accumulatedTime >= m_updatePeriod * m_maxUpdatesPerLoop) {
	//		m_accumulatedTime = m_updatePeriod * m_maxUpdatesPerLoop;
	//	}

	//	while (m_accumulatedTime >= m_updatePeriod)
	//	{
	//		m_accumulatedTime -= m_updatePeriod;
	//		if (m_accumulatedTime < (1.0 / 59.0) - m_updatePeriod) 
	//			m_accumulatedTime = 0;
	//		{
	//			rmt_ScopedCPUSample(MainThreadJobs, 0);
	//			//// Schedule the tasks
	//			ftl::AtomicCounter counter(taskScheduler);
	//			taskScheduler->AddTasks((uint)numTasks, tasks, &counter);

	//			// Wait for the tasks to complete
	//			taskScheduler->WaitForCounter(&counter, 0, true);
	//		}
	//	}

	//	rmt_BeginCPUSample(WaitForRenderer, 0);
	//	syncState->syncMTSempahore.Wait();
	//	syncState->syncRTSempahore.Signal();
	//	firstFrame = false;
	//	rmt_EndCPUSample();
	//}

	//delete[] tasks;

	////Destroy
	//delete Exp::Engine::m_Camera;
//}