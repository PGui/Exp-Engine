#include "InputModule.h"
#include <Remotery/Remotery.h>
#include "../Core/EngineModule.h"


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
		glfwSetCursorPosCallback(engineModule->mainWindow, Exp::InputModule::GlfwMouse);
		glfwSetKeyCallback(engineModule->mainWindow, Exp::InputModule::GlfwKey);
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
	return keysDown[key];
}


const bool Exp::InputModule::IsKeyReleased(int key)
{
	return !keysDown[key];
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
	}
}

//Static cb
void Exp::InputModule::GlfwMouse(GLFWwindow* window, double xpos, double ypos)
{
	/*if (InputModule * Input = ModuleManager::Get().GetModule<InputModule>("Input"))
	{
		
	}*/
}

void  Exp::InputModule::GlfwKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (InputModule * Input = ModuleManager::Get().GetModule<InputModule>("Input"))
	{
		if (action == GLFW_PRESS)
			Input->keysDown[key] = true;
		else if (action == GLFW_RELEASE)
			Input->keysDown[key] = false;
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