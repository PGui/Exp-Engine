#include "Engine.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include <memory_lib/tagged_heap.h>
#include "../Threading/Semaphore.h"

#include "../Rendering/RenderThread.h"

unsigned int Exp::Engine::m_screenWidth = 1280;
unsigned int Exp::Engine::m_screenHeight = 768;

Exp::GameSyncState * Exp::Engine::syncState = nullptr;

GLFWwindow*  Exp::Engine::m_mainWindow = nullptr;
GLFWwindow*  Exp::Engine::m_slaveWindow = nullptr;

float Exp::Engine::m_deltaTime = 1.0f / 60.0f;
float Exp::Engine::m_lastTime = 0.0f;
float Exp::Engine::m_accumulatedTime = 0.0f;

//Camera
Exp::Camera * Exp::Engine::m_Camera = nullptr;

//Debug render parameters
bool Exp::Engine::m_wireframe = false;
bool Exp::Engine::m_debugLight = false;

void Exp::Engine::MainJob(ftl::TaskScheduler * taskScheduler, void * arg)
{
	if (!glfwInit()) 
	{
		std::cout << "Failed to initialize glfw" << std::endl;
		return;
	}

	// Use https://gist.github.com/Madsy/6980061
	//main window
	Exp::Engine::m_mainWindow = InitWindow("Exp-Engine");
	//window used by second thread
	Exp::Engine::m_slaveWindow = InitWindow("", false, m_mainWindow, false);

	if (!Exp::Engine::m_mainWindow || !Exp::Engine::m_slaveWindow)
	{
		glfwTerminate();
		std::cout << "Failed to create glfw windows" << std::endl;
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

	// Loop until there is a quit message from the window or the user.
	while (!glfwWindowShouldClose(Exp::Engine::m_mainWindow)) {
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
		/*if (m_accumulatedTime >= updatePeriod * maxUpdatesPerLoop) {
			accumulatedTime = updatePeriod * maxUpdatesPerLoop;
		}
*/
		/*while (accumulatedTime >= updatePeriod) {
			accumulatedTime -= updatePeriod;
			Update(sceneNumber++, &heap);
		}*/

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		std::cout << "Update done. Launching rendering..." << std::endl;
		//Signal render
		Exp::Engine::syncState->syncSempahore.Signal();
	}

	Exp::Engine::syncState->shouldQuit.store(true);
	// Signal the render thread so it doesn't deadlock
	Exp::Engine::syncState->syncSempahore.Signal();

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
		m_Camera->setPerspective(m_Camera->_fovY, (float)width / (float)height, m_Camera->_nearPlane, m_Camera->_farPlane);
	}
}

void Exp::Engine::glfw_error_callback(int error, const char * description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
