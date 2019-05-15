#include "Engine.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include <memory_lib/tagged_heap.h>
#include "../Threading/Semaphore.h"

#include "../Rendering/RenderThread.h"

#include <Remotery/Remotery.h>

//Screen sizes
unsigned int Exp::Engine::m_screenWidth = 1280;
unsigned int Exp::Engine::m_screenHeight = 768;

//Sync State
Exp::GameSyncState * Exp::Engine::syncState = nullptr;

//Windows
GLFWwindow*  Exp::Engine::m_mainWindow = nullptr;
GLFWwindow*  Exp::Engine::m_slaveWindow = nullptr;

//Timers
float Exp::Engine::m_deltaTime = 1.0f / 60.0f;
float Exp::Engine::m_lastTime = 0.0f;
float Exp::Engine::m_accumulatedTime = 0.0f;
const double Exp::Engine::m_updatePeriod = 1.0f/60.0f;
const int Exp::Engine::m_maxUpdatesPerLoop = 4;

//Camera
Exp::Camera * Exp::Engine::m_Camera = nullptr;

//Debug render parameters
bool Exp::Engine::m_wireframe = false;
bool Exp::Engine::m_debugLight = false;

void Exp::Engine::MainJob(ftl::TaskScheduler * taskScheduler, void * arg)
{
	Remotery* rmt;
	rmt_CreateGlobalInstance(&rmt);

	rmt_SetCurrentThreadName("Main");

	if (!glfwInit()) 
	{
		std::cout << "Failed to initialize glfw" << std::endl;
		return;
	}

	// Use https://gist.github.com/Madsy/6980061
	//Check http://blog.slapware.eu/game-engine/programming/multithreaded-renderloop-part1/
	//main window
	Exp::Engine::m_mainWindow = InitWindow("Exp-Engine");
	//window used by second thread
	Exp::Engine::m_slaveWindow = InitWindow("", false, m_mainWindow, false);

	if (!Exp::Engine::m_mainWindow || !Exp::Engine::m_slaveWindow)
	{
		glfwTerminate();
		std::cout << "Failed to create glfw windows." << std::endl;
		return;
	}

	glfwMakeContextCurrent(Exp::Engine::m_mainWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize glad." << std::endl;
		glfwTerminate();
		return;
	}

	glfwSetInputMode(Exp::Engine::m_mainWindow, GLFW_CURSOR, GLFW_CURSOR);

	glfwSetErrorCallback(Exp::Engine::glfw_error_callback);
	glfwSetFramebufferSizeCallback(Exp::Engine::m_mainWindow, framebuffer_size_callback);

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	Exp::Engine::syncState = new GameSyncState();

	//Launch Render Thread
	ThreadArgs *renderArgs = new ThreadArgs;
	renderArgs->mainWindow = Exp::Engine::m_mainWindow;
	renderArgs->slaveWindow = Exp::Engine::m_slaveWindow;
	renderArgs->syncState = Exp::Engine::syncState;

	ftl::ThreadType renderThread;
	if (!ftl::CreateThread(1048576, RenderThreadStart, renderArgs, 0, &renderThread)) 
	{
		std::cout << "Failed to start the render thread" << std::endl;
	}

	//Init Cam
	Exp::Engine::m_Camera = new Camera(glm::vec3(0.0f));

	uint64 sceneNumber = 1;

	// Loop until there is a quit message from the window or the user.
	while (!glfwWindowShouldClose(Exp::Engine::m_mainWindow)) 
	{
		rmt_ScopedCPUSample(LogicLoop, 0);
		std::cout << "Start update." << std::endl;
		// Check for events
		glfwPollEvents();

		// Logic
		//rmt_ScopedCPUSample(LogicLoop, 0);

		float currentTime = (float)glfwGetTime();
		m_deltaTime = currentTime - m_lastTime;
		m_lastTime = currentTime;

		m_accumulatedTime += m_deltaTime;

		// Avoid spiral of death
		// Drop world time from the update in order to keep up
		if (m_accumulatedTime >= m_updatePeriod * m_maxUpdatesPerLoop) 
		{
			m_accumulatedTime = m_updatePeriod * m_maxUpdatesPerLoop;
		}

		while (m_accumulatedTime >= m_updatePeriod) 
		{
			m_accumulatedTime -= m_updatePeriod;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			//Update(sceneNumber++, &heap);
		}

		//std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		
		std::cout << "Update done. Launching rendering..." << std::endl;
		//Signal render
		rmt_BeginCPUSample(WaitForRenderer, 0);
		//Exp::Engine::syncState->syncSempahore.Signal();
		syncState->syncQueue.Push(sceneNumber++);
		rmt_EndCPUSample();

		std::this_thread::sleep_for(std::chrono::milliseconds(0));
	}

	Exp::Engine::syncState->shouldQuit.store(true);
	// Signal the render thread so it doesn't deadlock
	//Exp::Engine::syncState->syncSempahore.Signal();
	syncState->syncQueue.Push(0); //Force the render thread to end if it is waiting.

	// Wait for them to clean up
	ftl::JoinThread(renderThread);

	//Destroy
	delete Exp::Engine::m_Camera;
	delete Exp::Engine::syncState;

	glfwDestroyWindow(Exp::Engine::m_mainWindow);
	glfwDestroyWindow(Exp::Engine::m_slaveWindow);
	glfwTerminate();
}

GLFWwindow * Exp::Engine::InitWindow(std::string title, bool fullScreen, GLFWwindow* shared, bool visible)
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

void Exp::Engine::framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	m_screenWidth = width;
	m_screenHeight = height;
	glViewport(0, 0, m_screenWidth, m_screenHeight);
	if (m_Camera)
	{
		m_Camera->SetPerspective(m_Camera->m_fovY, (float)width / (float)height, m_Camera->m_nearPlane, m_Camera->m_farPlane);
	}
}

void Exp::Engine::glfw_error_callback(int error, const char * description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
