#include "../Input/InputModule.h"
#include <Remotery/Remotery.h>
#include "Camera.h"

namespace Exp
{
	Camera::Camera(glm::vec3 position, glm::vec3 forward, glm::vec3 up) :
		position(position),
		forward(forward),
		up(up)

	{
		worldUp = up;
		targetPosition = position;
		targetYaw = yaw;
		targetPitch = pitch;


		UpdateView();

		SetPerspective(fovY, aspectRatio, nearPlane, farPlane);
	}

	void Camera::Update(float deltatime, InputModule * inputModule)
	{
		rmt_ScopedCPUSample(CameraUpdate, 0);

		if (inputModule)
		{
			if (inputModule->IsKeyPressed(GLFW_KEY_W))
				UpdateKey((float)deltatime, CameraMovement::FORWARD);
			if (inputModule->IsKeyPressed(GLFW_KEY_S))
				UpdateKey((float)deltatime, CameraMovement::BACKWARD);
			if (inputModule->IsKeyPressed(GLFW_KEY_A))
				UpdateKey((float)deltatime, CameraMovement::LEFT);
			if (inputModule->IsKeyPressed(GLFW_KEY_D))
				UpdateKey((float)deltatime, CameraMovement::RIGHT);
			if (inputModule->IsKeyPressed(GLFW_KEY_SPACE))
				UpdateKey((float)deltatime, CameraMovement::UP);
			if (inputModule->IsKeyPressed(GLFW_KEY_LEFT_CONTROL))
				UpdateKey((float)deltatime, CameraMovement::DOWN);
			glm::vec2 mouseDelta = inputModule->GetMouseDelta();
			UpdateMouse(mouseDelta.x, mouseDelta.y);
		}

		position = glm::lerp(position, targetPosition, glm::clamp(1.f - pow(1.f - damping / 60.f, 60.f * deltatime), 0.0f, 1.0f));
		yaw = glm::lerp(yaw, targetYaw, glm::clamp(1.f - pow(1.f - damping * 2.0f / 60.f, 60.f * deltatime), 0.0f, 1.0f));
		pitch = glm::lerp(pitch, targetPitch, glm::clamp(1.f - pow(1.f - damping * 2.0f / 60.f, 60.f * deltatime), 0.0f, 1.0f));

		glm::vec3 newForward;
		newForward.x = float(cos(0.0174533 * pitch) * cos(0.0174533 * yaw));
		newForward.y = float(sin(0.0174533 * pitch));
		newForward.z = float(cos(0.0174533 * pitch) * sin(0.0174533 * yaw));
		forward = glm::normalize(newForward);
		right = glm::normalize(glm::cross(forward, worldUp));
		up = glm::cross(right, forward);

		UpdateView();
	}

	void Camera::UpdateMouse(float deltaX, float deltaY)
	{
		if (disableMouse)
			return;

		float x = deltaX * sensitivity;
		float y = deltaY * sensitivity;

		targetYaw -= x;
		targetPitch += y;

		if (targetYaw == 0.0f) targetYaw = 0.01f;
		if (targetPitch == 0.0f) targetPitch = 0.01f;

		if (targetPitch > 89.0f)  targetPitch = 89.0f;
		if (targetPitch < -89.0f) targetPitch = -89.0f;
	}

	void Camera::UpdateKey(float deltaTime, CameraMovement direction)
	{
		float currentSpeed = speed * deltaTime;
		if (direction == CameraMovement::FORWARD)
			targetPosition = targetPosition + forward * currentSpeed;
		else if (direction == CameraMovement::BACKWARD)
			targetPosition = targetPosition - forward * currentSpeed;
		else if (direction == CameraMovement::LEFT)
			targetPosition = targetPosition - right * currentSpeed;
		else if (direction == CameraMovement::RIGHT)
			targetPosition = targetPosition + right * currentSpeed;
		else if (direction == CameraMovement::UP)
			targetPosition = targetPosition + worldUp * currentSpeed;
		else if (direction == CameraMovement::DOWN)
			targetPosition = targetPosition - worldUp * currentSpeed;
	}

	void Camera::UpdateView()
	{
		view = LookAt(position, position + forward, up);
		viewProjection = projection * view;
	}

	glm::mat4 Camera::LookAt(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)
	{
		//Compute new axis
		glm::vec3 zAxis = glm::normalize(position - target);
		glm::vec3 xAxis = glm::normalize(glm::cross(glm::normalize(worldUp), zAxis));
		glm::vec3 yAxis = glm::cross(zAxis, xAxis);

		glm::mat4 translation(1.0f);
		translation[3][0] = -position.x;
		translation[3][1] = -position.y;
		translation[3][2] = -position.z;

		glm::mat4 rotation(1.0f);
		rotation[0][0] = xAxis.x;
		rotation[1][0] = xAxis.y;
		rotation[2][0] = xAxis.z;

		rotation[0][1] = yAxis.x;
		rotation[1][1] = yAxis.y;
		rotation[2][1] = yAxis.z;

		rotation[0][2] = zAxis.x;
		rotation[1][2] = zAxis.y;
		rotation[2][2] = zAxis.z;

		return rotation * translation;
	}

	void Camera::SetPerspective(float fovY, float aspectRatio, float nearPlane, float farPlane)
	{
		fovY = fovY;
		aspectRatio = aspectRatio;
		nearPlane = nearPlane;
		farPlane = farPlane;
		projection = glm::perspective(glm::radians(fovY), aspectRatio, nearPlane, farPlane);
	}


	Camera::~Camera()
	{
	}

}
