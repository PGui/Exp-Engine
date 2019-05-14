#pragma once

#include <glad/glad.h>
#include "../glm/glm.hpp"
#include "../glm/gtx/compatibility.hpp"
#include "../glm/gtc/matrix_transform.hpp"

namespace Exp
{
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
		glm::mat4 _view = glm::mat4(1.0f);
		glm::mat4 _projection = glm::mat4(1.0f);

		glm::mat4 _viewProjection = glm::mat4(1.0f);

		glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 _forward = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 _right = glm::vec3(1.0f, 0.0f, 0.0f);

		float _fovY = 50.0f;
		float _aspectRatio = 16.0f / 9.0f;
		float _nearPlane = 0.1f;
		float _farPlane = 2000.0f;

		float _yaw = -90.0f;
		float _pitch = 0.0f;

		float _speed = 10.0f;
		float _sensitivity = 0.3f;

		float _damping = 5.0f;

		bool _disableMouse = false;

	private:
		float _targetYaw = 0.0f;
		float _targetPitch = 0.0f;

		glm::vec3 _worldUp;
		glm::vec3 _targetPosition;

	public:
		~Camera();

		Camera(glm::vec3 position, glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));

		void update(float deltatime);

		void updateMouse(float deltaX, float deltaY);
		void updateKey(float deltaTime, CameraMovement direction);

		void setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);

	private:
		void updateView();

		glm::mat4 lookAt(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp);

	};
}
