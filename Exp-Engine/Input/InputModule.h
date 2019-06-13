#pragma once
#include "../Module/ModuleManager.h"
#include "../Module/ModuleInterface.h"
#include "../Core/CoreModule.h"
#include <iostream>

#include "../Glm/glm.hpp"


namespace Exp
{
	class InputModule :
		public IModuleInterface
	{
	private:

		glm::vec2 lastPos = glm::vec2();
		glm::vec2 delta = glm::vec2();

		bool keysDown[512];

		GLFWwindow * window = nullptr;

	public:
		InputModule();
		virtual ~InputModule();

		virtual void StartUp();

		virtual void Shutdown();

		//Static cb
		static void glfw_mouse(GLFWwindow* window, double xpos, double ypos);
		static void glfw_key(GLFWwindow* window, int key, int scancode, int action, int mods);

	public:
		const glm::vec2 GetMouseDelta();

		//Todo : Come up with something better, but that'll do for now.
		const bool IsKeyPressed(int key);
		const bool IsKeyReleased(int key);
		const bool KeyPressed(int key);
		const bool KeyReleased(int key);

		void Update(const float & deltaTime);
	};
}

