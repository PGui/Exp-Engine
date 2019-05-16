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
double Exp::Engine::m_deltaTime = 0.0;
double Exp::Engine::m_lastTime = 0.0;
double Exp::Engine::m_accumulatedTime = 0.0;
const double Exp::Engine::m_updatePeriod = 1.0/60.0;
const double Exp::Engine::m_precisionUpdatePeriod = 1.0 / 59.0;
const int Exp::Engine::m_maxUpdatesPerLoop = 2;

//Camera
Exp::Camera * Exp::Engine::m_Camera = nullptr;

//Debug render parameters
bool Exp::Engine::m_wireframe = false;
bool Exp::Engine::m_debugLight = false;

void Exp::Engine::MainJob(ftl::TaskScheduler * taskScheduler, void * arg)
{
	ThreadArgs *logicArgs = static_cast<ThreadArgs *>(arg);
	GLFWwindow *window = logicArgs->mainWindow;
	GameSyncState *syncState = logicArgs->syncState;
	Semaphore *syncSemaphore = logicArgs->syncSemaphore;
	delete logicArgs;

	//Init Cam
	Exp::Engine::m_Camera = new Camera(glm::vec3(0.0f));

	uint64 sceneNumber = 1;

	glfwMakeContextCurrent(nullptr);

	size_t numTasks = 15;
	ftl::Task *tasks = new ftl::Task[numTasks];
	int *numbers = new int[numTasks];
	for (uint64 i = 0ull; i < numTasks; ++i) 
	{
		numbers[i] = (int)i;
		tasks[i] = { Exp::Engine::ParallelJob , &numbers[i] };
	}

	m_lastTime = glfwGetTime();

	syncState->syncMTSempahore.Signal(); //Don't make the MT Wait first frame.

	bool firstFrame = true;

	// Loop until there is a quit message from the window or the user.
	while (!glfwWindowShouldClose(Exp::Engine::m_mainWindow))
	{
		rmt_ScopedCPUSample(LogicLoop, 0);

		rmt_BeginCPUSample(PollEvents, 0);
		// Check for events
		glfwPollEvents();
		rmt_EndCPUSample();

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

		while (m_accumulatedTime >= m_updatePeriod)
		{
			m_accumulatedTime -= m_updatePeriod;
			if (m_accumulatedTime < (1.0 / 59.0) - m_updatePeriod) 
				m_accumulatedTime = 0;
			{
				rmt_ScopedCPUSample(MainThreadJobs, 0);
				//// Schedule the tasks
				ftl::AtomicCounter counter(taskScheduler);
				taskScheduler->AddTasks((uint)numTasks, tasks, &counter);

				// Wait for the tasks to complete
				taskScheduler->WaitForCounter(&counter, 0, true);
			}
		}

		rmt_BeginCPUSample(WaitForRenderer, 0);
		syncState->syncMTSempahore.Wait();
		syncState->syncRTSempahore.Signal();
		firstFrame = false;
		rmt_EndCPUSample();
	}

	delete[] tasks;

	//Destroy
	delete Exp::Engine::m_Camera;
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
