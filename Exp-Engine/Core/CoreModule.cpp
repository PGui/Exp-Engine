#include "CoreModule.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include <memory_lib/tagged_heap.h>
#include "../Threading/Semaphore.h"

#include "../Rendering/RenderThread.h"

#include <Remotery/Remotery.h>

GLFWwindow * Exp::CoreModule::InitWindow(std::string title, bool fullScreen, GLFWwindow* shared, bool visible)
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
void Exp::CoreModule::RunEngine()
{
	while (!glfwWindowShouldClose(m_mainWindow))
	{
		rmt_ScopedCPUSample(LogicLoop, 0);

		rmt_BeginCPUSample(PollEvents, 0);
		// Check for events
		glfwPollEvents();
		rmt_EndCPUSample();

		NewFrame();

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

		//while (m_accumulatedTime >= m_updatePeriod)
		//{
		//	m_accumulatedTime -= m_updatePeriod;
		//	if (m_accumulatedTime < (1.0 / 59.0) - m_updatePeriod) 
		//		m_accumulatedTime = 0;
		//	{
		//		rmt_ScopedCPUSample(MainThreadJobs, 0);
		//		//// Schedule the tasks
		//		ftl::AtomicCounter counter(taskScheduler);
		//		taskScheduler->AddTasks((uint)numTasks, tasks, &counter);

		//		// Wait for the tasks to complete
		//		taskScheduler->WaitForCounter(&counter, 0, true);
		//	}
		//}

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
		}

		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui::EndFrame();
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(m_mainWindow);
	}
}

void Exp::CoreModule::NewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Exp::CoreModule::framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
}
void Exp::CoreModule::glfw_error_callback(int error, const char * description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void Exp::CoreModule::StartUp()
{
	if (!glfwInit())
	{
		std::cout << "Failed to initialize glfw" << std::endl;
		return;
	}

	//main window
	m_mainWindow = InitWindow("Exp-Engine");

	if (!Exp::CoreModule::m_mainWindow )
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

	glfwSetInputMode(Exp::CoreModule::m_mainWindow, GLFW_CURSOR, GLFW_CURSOR);

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
}

void Exp::CoreModule::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_mainWindow);
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


//
//void Exp::Engine::framebuffer_size_callback(GLFWwindow * window, int width, int height)
//{
//	m_screenWidth = width;
//	m_screenHeight = height;
//	glViewport(0, 0, m_screenWidth, m_screenHeight);
//	if (m_Camera)
//	{
//		m_Camera->SetPerspective(m_Camera->m_fovY, (float)width / (float)height, m_Camera->m_nearPlane, m_Camera->m_farPlane);
//	}
//}
//
//void Exp::Engine::glfw_error_callback(int error, const char * description)
//{
//	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
//}