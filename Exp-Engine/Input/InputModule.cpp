#include "InputModule.h"



Exp::InputModule::InputModule()
{
}


Exp::InputModule::~InputModule()
{
}

void Exp::InputModule::StartUp()
{
	if (CoreModule * Core = Exp::ModuleManager::Get().GetModule<CoreModule>("Core"))
	{
		window = Core->m_mainWindow;
		glfwSetCursorPosCallback(Core->m_mainWindow, Exp::InputModule::glfw_mouse);
		glfwSetKeyCallback(Core->m_mainWindow, Exp::InputModule::glfw_key);
	}
	else
	{
		std::cout << "Error 'Core' Module not initialized." << std::endl;
	}
}

void Exp::InputModule::Shutdown()
{
}

const glm::vec2 Exp::InputModule::GetMouseDelta()
{
	return delta;
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

		delta = glm::vec2((float)(lastPos.x - xpos), (float)(lastPos.y - ypos));
		lastPos = glm::vec2(xpos, ypos);
	}
	
}

//Static cb
void Exp::InputModule::glfw_mouse(GLFWwindow* window, double xpos, double ypos)
{
	/*if (InputModule * Input = ModuleManager::Get().GetModule<InputModule>("Input"))
	{
		
	}*/
}

void  Exp::InputModule::glfw_key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (InputModule * Input = ModuleManager::Get().GetModule<InputModule>("Input"))
	{
		if (action == GLFW_PRESS)
			Input->keysDown[key] = true;
		else if (action == GLFW_RELEASE)
			Input->keysDown[key] = false;
	}

	/*if (key == GLFW_KEY_F1 && action == GLFW_RELEASE)
	{
		EngineRenderer->m_Camera->_disableMouse = !EngineRenderer->m_Camera->_disableMouse;
		if (EngineRenderer->m_Camera->_disableMouse)
		{
			glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}*/
}

//void Exp::ProcessInput()
//{
//	/*if (glfwGetKey(g_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//	{
//		glfwSetWindowShouldClose(g_Window, true);
//	}
//
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
//		EngineRenderer->m_Camera->updateKey(deltaTime, CameraMovement::DOWN);*/
//}