#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Exp
{
	class InputModule;

	enum CameraMovement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	class Camera
	{
	public:
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		glm::mat4 viewProjection = glm::mat4(1.0f);

		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);

		float fovY = 50.0f;
		float aspectRatio = 16.0f / 9.0f;
		float nearPlane = 0.1f;
		float farPlane = 2000.0f;

		float yaw = -90.0f;
		float pitch = 0.0f;

		float speed = 10.0f;
		float sensitivity = 0.3f;

		float damping = 5.0f;

		bool disableMouse = false;

	private:
		float targetYaw = 0.0f;
		float targetPitch = 0.0f;

		glm::vec3 worldUp;
		glm::vec3 targetPosition;

	public:
		~Camera();

		Camera(glm::vec3 position, glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));

		void Update(float deltatime, InputModule* inputModule);

		void UpdateMouse(float deltaX, float deltaY);
		void UpdateKey(float deltaTime, CameraMovement direction);

		void SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);

	private:
		void UpdateView();

		glm::mat4 LookAt(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp);

	};
}
