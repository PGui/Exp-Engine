#include "EngineModule.h"
#include "Application.h"
#include <cmath>
#include <chrono>
#include <thread>
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
	const double runStartTime = lastTime;
	accumulatedTime = 0.0;

	int frame = 0;
	while (!glfwWindowShouldClose(mainWindow) && (maxFrames <= 0 || frame < maxFrames))
	{
		rmt_ScopedCPUSample(LogicLoop, 0);
		const double frameStartTime = glfwGetTime();

		rmt_BeginCPUSample(PollEvents, 0);
		glfwPollEvents();
		rmt_EndCPUSample();

		NewFrame();

		Update();

		ModuleManager::Get().DisplayModulesUI();
		application->OnUI(*this);

		// Gather renderables from the ECS (interpolated between fixed steps), then draw.
		const bool interpolate = simulationMode == SimulationMode::FixedInterpolated;
		ecsModule->SubmitRenderables(interpolate ? interpolationAlpha : 1.0f, interpolate);
		renderingModule->Render();

		EndFrame();
		LimitFrameRate(frameStartTime);
		++frame;
	}

	const double elapsed = glfwGetTime() - runStartTime;
	if (frame > 0 && elapsed > 0.0)
	{
		spdlog::info("{} frames in {:.2f}s ({:.1f} FPS avg), simulated {:.2f}s, mode: {}",
			frame, elapsed, frame / elapsed, simulationTime,
			simulationMode == SimulationMode::Variable ? "variable" : "fixed + interpolation");
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

	if (simulationMode == SimulationMode::Variable)
	{
		// One simulation step per rendered frame, with the real (clamped) frame time.
		rmt_ScopedCPUSample(VariableUpdate, 0);
		ecsModule->FixedUpdate(dt);
		application->OnFixedUpdate(*this, dt);
		simulationTime += deltaTime;
		accumulatedTime = 0.0;
		interpolationAlpha = 1.0f;
		return;
	}

	// Fixed-step simulation: consume the accumulated time in constant steps so
	// gameplay is frame-rate independent and deterministic. Rendering runs at any
	// rate and interpolates between the last two steps (interpolationAlpha).
	const float fixedDt = static_cast<float>(updatePeriod);
	while (accumulatedTime >= updatePeriod)
	{
		rmt_ScopedCPUSample(FixedUpdate, 0);
		ecsModule->FixedUpdate(fixedDt);
		application->OnFixedUpdate(*this, fixedDt);
		accumulatedTime -= updatePeriod;
		simulationTime += updatePeriod;
	}
	interpolationAlpha = static_cast<float>(accumulatedTime / updatePeriod);
}

void Exp::EngineModule::SetVSync(bool enabled)
{
	vsync = enabled;
	glfwSwapInterval(enabled ? 1 : 0);
}

void Exp::EngineModule::LimitFrameRate(double frameStartTime)
{
	if (vsync || maxFps <= 0)
		return;

	const double targetFrameTime = 1.0 / maxFps;
	// Sleep most of the remaining time, then spin for precision.
	double remaining = targetFrameTime - (glfwGetTime() - frameStartTime);
	if (remaining > 0.002)
		std::this_thread::sleep_for(std::chrono::duration<double>(remaining - 0.001));
	while (glfwGetTime() - frameStartTime < targetFrameTime)
	{
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
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	frameTimes[frameTimeIndex] = static_cast<float>(deltaTime * 1000.0);
	frameTimeIndex = (frameTimeIndex + 1) % frameHistorySize;

	// Long frames (breakpoint, window drag...) must not explode the simulation.
	if (deltaTime > maxDeltaTime)
		deltaTime = maxDeltaTime;

	// Snap to common refresh rates to reduce jitter from timer noise.
	for (double rate : { 1.0 / 240.0, 1.0 / 165.0, 1.0 / 144.0, 1.0 / 120.0, 1.0 / 75.0, 1.0 / 60.0, 1.0 / 30.0 })
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
	SetVSync(vsync);

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
		ImGui::PlotLines("##frametimes", frameTimes, frameHistorySize, frameTimeIndex,
			"frame time (ms)", 0.0f, 50.0f, ImVec2(0, 50));

		bool vsyncEnabled = vsync;
		if (ImGui::Checkbox("VSync", &vsyncEnabled))
			SetVSync(vsyncEnabled);
		if (!vsync)
			ImGui::SliderInt("Max FPS (0 = off)", &maxFps, 0, 500);

		int mode = static_cast<int>(simulationMode);
		const char* modes[] = { "Fixed + interpolation", "Variable" };
		if (ImGui::Combo("Simulation", &mode, modes, 2))
		{
			simulationMode = static_cast<SimulationMode>(mode);
			accumulatedTime = 0.0;
		}
		if (simulationMode == SimulationMode::FixedInterpolated)
		{
			static const int rates[] = { 20, 30, 60, 120, 240 };
			static const char* rateNames[] = { "20 Hz", "30 Hz", "60 Hz", "120 Hz", "240 Hz" };
			int current = 2;
			for (int i = 0; i < 5; ++i)
				if (std::abs(updatePeriod - 1.0 / rates[i]) < 1e-9) current = i;
			if (ImGui::Combo("Fixed rate", &current, rateNames, 5))
				updatePeriod = 1.0 / rates[current];
			ImGui::Text("Interpolation alpha: %.2f", interpolationAlpha);
		}
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