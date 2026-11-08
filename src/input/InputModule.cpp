#include "InputModule.h"
#include "Remotery.h"
#include "../core/EngineModule.h"
#include "imgui.h"


Exp::InputModule::InputModule()
{
}


Exp::InputModule::~InputModule()
{
}

void Exp::InputModule::StartUp()
{
	rmt_ScopedCPUSample(InputModuleStartUp, 0);

	spdlog::info("InputModule StartUp");
}

void Exp::InputModule::Shutdown()
{
	spdlog::info("InputModule Shutdown");
}

void Exp::InputModule::PostInitialize()
{
	engineModule = Exp::ModuleManager::Get().GetModule<EngineModule>("Engine");
	if (engineModule)
	{
		window = engineModule->mainWindow;
		// ImGui's GLFW backend installed its callbacks during the EngineModule startup.
		// Keep them and chain to them: since ImGui 1.87 the backend is purely event
		// driven, so replacing them leaves ImGui without mouse position / keyboard.
		previousCursorPosCallback = glfwSetCursorPosCallback(engineModule->mainWindow, Exp::InputModule::GlfwMouse);
		previousKeyCallback = glfwSetKeyCallback(engineModule->mainWindow, Exp::InputModule::GlfwKey);
	}
	else
	{
		spdlog::critical("InputModule Error : 'Core' Module not initialized");
	}
}

const glm::vec2 & Exp::InputModule::GetMouseDelta()
{
	return mouseDelta;
}

const bool Exp::InputModule::IsKeyPressed(int key)
{
	return key >= 0 && key < kMaxKeys && keysDown[key];
}


const bool Exp::InputModule::IsKeyReleased(int key)
{
	return !IsKeyPressed(key);
}

const bool Exp::InputModule::KeyPressed(int key)
{
	return false;
}

const bool Exp::InputModule::KeyReleased(int key)
{
	return false;
}

void Exp::InputModule::Update(const float & deltaTime)
{
	rmt_ScopedCPUSample(InputModuleUpdate, 0);
	//Mouse delta
	if (window)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		static bool firstMouse = true;
		if (firstMouse)
		{
			lastPos = glm::vec2(xpos, ypos);
			firstMouse = false;
		}

		mouseDelta = glm::vec2((float)(lastPos.x - xpos), (float)(lastPos.y - ypos));
		lastPos = glm::vec2(xpos, ypos);

		// The mouse is over / dragging an ImGui widget: don't rotate the camera.
		const bool cursorCaptured = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
		if (!cursorCaptured && ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureMouse)
			mouseDelta = glm::vec2(0.0f);
	}
}

//Static cb
GLFWcursorposfun Exp::InputModule::previousCursorPosCallback = nullptr;
GLFWkeyfun Exp::InputModule::previousKeyCallback = nullptr;

void Exp::InputModule::GlfwMouse(GLFWwindow* window, double xpos, double ypos)
{
	if (previousCursorPosCallback)
		previousCursorPosCallback(window, xpos, ypos); // ImGui
}

void  Exp::InputModule::GlfwKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (previousKeyCallback)
		previousKeyCallback(window, key, scancode, action, mods); // ImGui

	if (InputModule * Input = ModuleManager::Get().GetModule<InputModule>("Input"))
	{
		if (key >= 0 && key < kMaxKeys)
		{
			// Typing in an ImGui text field must not move the camera,
			// but releases are always recorded so no key gets stuck.
			const bool imguiWantsKeyboard = ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureKeyboard;
			if (action == GLFW_PRESS && !imguiWantsKeyboard)
				Input->keysDown[key] = true;
			else if (action == GLFW_RELEASE)
				Input->keysDown[key] = false;
		}
	}

	if (key == GLFW_KEY_F1 && action == GLFW_RELEASE)
	{
		if (EngineModule * Engine = ModuleManager::Get().GetModule<EngineModule>("Engine"))
		{
			Engine->camera.disableMouse = !Engine->camera.disableMouse;
			if (Engine->camera.disableMouse)
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			else
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
		}

	}
}

//void Exp::ProcessInput()
//{
//	/*if (glfwGetKey(g_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//	{
//		glfwSetWindowShouldClose(g_Window, true);
//	}
//
//	if (glfwGetKey(g_Window, GLFW_KEY_W) == GLFW_PRESS)
//		EngineRenderer->camera->updateKey(deltaTime, CameraMovement::FORWARD);
//	if (glfwGetKey(g_Window, GLFW_KEY_S) == GLFW_PRESS)
//		EngineRenderer->camera->updateKey(deltaTime, CameraMovement::BACKWARD);
//	if (glfwGetKey(g_Window, GLFW_KEY_A) == GLFW_PRESS)
//		EngineRenderer->camera->updateKey(deltaTime, CameraMovement::LEFT);
//	if (glfwGetKey(g_Window, GLFW_KEY_D) == GLFW_PRESS)
//		EngineRenderer->camera->updateKey(deltaTime, CameraMovement::RIGHT);
//	if (glfwGetKey(g_Window, GLFW_KEY_SPACE) == GLFW_PRESS)
//		EngineRenderer->camera->updateKey(deltaTime, CameraMovement::UP);
//	if (glfwGetKey(g_Window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
//		EngineRenderer->camera->updateKey(deltaTime, CameraMovement::DOWN);*/
//}