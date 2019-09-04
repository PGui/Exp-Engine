#pragma once

#include <glad/glad.h>
#include "../glm/glm.hpp"
#include "../glm/gtx/compatibility.hpp"
#include "../glm/gtc/matrix_transform.hpp"


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
		glm::mat4 m_view = glm::mat4(1.0f);
		glm::mat4 m_projection = glm::mat4(1.0f);

		glm::mat4 m_viewProjection = glm::mat4(1.0f);

		glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_forward = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 m_right = glm::vec3(1.0f, 0.0f, 0.0f);

		float m_fovY = 50.0f;
		float m_aspectRatio = 16.0f / 9.0f;
		float m_nearPlane = 0.1f;
		float m_farPlane = 2000.0f;

		float m_yaw = -90.0f;
		float m_pitch = 0.0f;

		float m_speed = 10.0f;
		float m_sensitivity = 0.3f;

		float m_damping = 5.0f;

		bool m_disableMouse = false;

	private:
		float m_targetYaw = 0.0f;
		float m_targetPitch = 0.0f;

		glm::vec3 m_worldUp;
		glm::vec3 m_targetPosition;

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
