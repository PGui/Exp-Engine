#include "EngineModule.h"
#include "Application.h"
#include <cmath>
#include <glad/glad.h>


#include "Remotery.h"

#include "../module/ModuleManager.h"
#include "../rendering/RenderingModule.h"
#include "../input/InputModule.h"
#include "../materialLibrary/MaterialLibraryModule.h"
#include "ecs/ECSModule.h"

#include "../scene/SceneNode.h"
#include "../scene/Scene.h"


#include "WindowParameters.h"

#include <GLFW/glfw3.h>



GLFWwindow * Exp::EngineModule::InitWindow(std::string title, bool fullScreen, GLFWwindow* shared, bool visible)
{
	GLFWwindow* win;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef NDEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

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

	win = glfwCreateWindow(Exp::WinParameters.screenWidth, Exp::WinParameters.screenHeight, title.c_str(), monitor, shared);

	return win;
}
void Exp::EngineModule::RunEngine(Application& app, int maxFrames)
{
	application = &app;
	application->OnStart(*this);

	lastTime = glfwGetTime();
	accumulatedTime = 0.0;

	int frame = 0;
	while (!glfwWindowShouldClose(mainWindow) && (maxFrames <= 0 || frame < maxFrames))
	{
		rmt_ScopedCPUSample(LogicLoop, 0);

		rmt_BeginCPUSample(PollEvents, 0);
		glfwPollEvents();
		rmt_EndCPUSample();

		NewFrame();

		Update();

		ModuleManager::Get().DisplayModulesUI();
		application->OnUI(*this);

		// Gather renderables from the ECS, then draw.
		ecsModule->SubmitRenderables();
		renderingModule->Render();

		EndFrame();
		++frame;
	}

	application->OnShutdown(*this);
	application = nullptr;
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

	const float dt = static_cast<float>(deltaTime);
	inputModule->Update(dt);
	camera.Update(dt, inputModule);

	application->OnUpdate(*this, dt);

	// Fixed-step simulation: consume the accumulated time in constant steps so
	// gameplay is frame-rate independent and deterministic.
	const float fixedDt = static_cast<float>(updatePeriod);
	while (accumulatedTime >= updatePeriod)
	{
		rmt_ScopedCPUSample(FixedUpdate, 0);
		ecsModule->FixedUpdate(fixedDt);
		application->OnFixedUpdate(*this, fixedDt);
		accumulatedTime -= updatePeriod;
		simulationTime += updatePeriod;
	}
}

void Exp::EngineModule::EndFrame()
{
	ImGui::EndFrame();
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(mainWindow);
}

void Exp::EngineModule::ComputeDeltatime()
{
	// Logic
	double currentTime = glfwGetTime();
	deltaTime = (currentTime - lastTime);/* / updatePeriod;*/
	lastTime = currentTime;

	// Snap to common refresh rates to reduce jitter from timer noise.
	for (double rate : { 1.0 / 144.0, 1.0 / 120.0, 1.0 / 60.0, 1.0 / 30.0 })
	{
		if (std::abs(deltaTime - rate) < .0002)
		{
			deltaTime = rate;
			break;
		}
	}

	accumulatedTime += deltaTime;

	// Avoid the spiral of death: if we are too slow, drop simulation time
	// instead of running an ever growing number of fixed steps.
	if (accumulatedTime > updatePeriod * maxUpdatesPerLoop) {
		accumulatedTime = updatePeriod * maxUpdatesPerLoop;
	}
}

void Exp::EngineModule::FramebufferSizeCallback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
	//TODO put this callback somewhere more global
	if (EngineModule * Core = ModuleManager::Get().GetModule<Exp::EngineModule>("Engine"))
	{
		Core->camera.SetPerspective(
			Core->camera.fovY,
			(float)width / (float)height,
			Core->camera.nearPlane,
			Core->camera.farPlane);
	}

	if (RenderingModule * Renderer = ModuleManager::Get().GetModule<Exp::RenderingModule>("Rendering"))
	{
		Renderer->ResizeRenderer(width, height);
	}
}
void APIENTRY Exp::EngineModule::GlDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	if (severity == GL_DEBUG_SEVERITY_HIGH || type == GL_DEBUG_TYPE_ERROR)
		spdlog::error("[GL] {}", message);
	else
		spdlog::warn("[GL] {}", message);
}

void Exp::EngineModule::GlfwErrorCallback(int error, const char * description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void Exp::EngineModule::StartUp()
{
	rmt_ScopedCPUSample(EngineModuleStartUp, 0);

	if (!glfwInit())
	{
		spdlog::critical("Failed to initialize glfw");
		return;
	}

	//main window
	mainWindow = InitWindow(Exp::WinParameters.title);

	if (!Exp::EngineModule::mainWindow)
	{
		glfwTerminate();
		spdlog::critical("Failed to create glfw window");
		return;
	}

	glfwMakeContextCurrent(mainWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		spdlog::critical("Failed to initialize glad");
		glfwTerminate();
		return;
	}

#ifndef NDEBUG
	if (GLAD_GL_VERSION_4_3)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(GlDebugCallback, nullptr);
		// Notifications are too verbose
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
	}
#endif

	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR);

	glfwSetFramebufferSizeCallback(mainWindow, FramebufferSizeCallback);

	// Setup Dear ImGui context
	const char* glsl_version = "#version 450";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(mainWindow, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	Resources::Init(); // TODO Make a module

	spdlog::info("EngineModule StartUp");
}

void Exp::EngineModule::Shutdown()
{
	// Engine-owned GPU resources go first, while every module is still alive.
	Resources::Clean(); //TODO Make a module

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// The window (and its GL context) is destroyed in ~EngineModule: modules registered
	// after the engine (e.g. Rendering) still release GL objects in their destructors.
	spdlog::info("EngineModule Shutdown");
}

Exp::EngineModule::~EngineModule()
{
	if (mainWindow)
	{
		glfwDestroyWindow(mainWindow);
		mainWindow = nullptr;
	}
	glfwTerminate();
}

void Exp::EngineModule::DisplayUI()
{
	if (ImGui::CollapsingHeader("Core", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Checkbox("Disable Mouse (F1)", &camera.disableMouse);
	}
}

void Exp::EngineModule::PostInitialize()
{
	inputModule = ModuleManager::Get().GetModule<InputModule>("Input");
	renderingModule = ModuleManager::Get().GetModule<RenderingModule>("Rendering");
	materialLibraryModule = ModuleManager::Get().GetModule<MaterialLibraryModule>("MaterialLibrary");
	ecsModule = ModuleManager::Get().GetModule<ECSModule>("ECS");

	renderingModule->SetCamera(&camera);

}