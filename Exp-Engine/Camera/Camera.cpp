#include "Camera.h"

namespace Exp
{
	Camera::Camera(glm::vec3 position, glm::vec3 forward, glm::vec3 up) :
		m_position(position),
		m_forward(forward),
		m_up(up)

	{
		m_worldUp = m_up;
		m_targetPosition = m_position;
		m_targetYaw = m_yaw;
		m_targetPitch = m_pitch;


		UpdateView();

		SetPerspective(m_fovY, m_aspectRatio, m_nearPlane, m_farPlane);
	}

	void Camera::Update(float deltatime)
	{
		m_position = glm::lerp(m_position, m_targetPosition, glm::clamp(1.f - pow(1.f - m_damping / 60.f, 60.f * deltatime), 0.0f, 1.0f));
		m_yaw = glm::lerp(m_yaw, m_targetYaw, glm::clamp(1.f - pow(1.f - m_damping * 2.0f / 60.f, 60.f * deltatime), 0.0f, 1.0f));
		m_pitch = glm::lerp(m_pitch, m_targetPitch, glm::clamp(1.f - pow(1.f - m_damping * 2.0f / 60.f, 60.f * deltatime), 0.0f, 1.0f));

		glm::vec3 newForward;
		newForward.x = float(cos(0.0174533 * m_pitch) * cos(0.0174533 * m_yaw));
		newForward.y = float(sin(0.0174533 * m_pitch));
		newForward.z = float(cos(0.0174533 * m_pitch) * sin(0.0174533 * m_yaw));
		m_forward = glm::normalize(newForward);
		m_right = glm::normalize(glm::cross(m_forward, m_worldUp));
		m_up = glm::cross(m_right, m_forward);

		UpdateView();
	}

	void Camera::UpdateMouse(float deltaX, float deltaY)
	{
		if (m_disableMouse)
			return;

		float x = deltaX * m_sensitivity;
		float y = deltaY * m_sensitivity;

		m_targetYaw -= x;
		m_targetPitch += y;

		if (m_targetYaw == 0.0f) m_targetYaw = 0.01f;
		if (m_targetPitch == 0.0f) m_targetPitch = 0.01f;

		if (m_targetPitch > 89.0f)  m_targetPitch = 89.0f;
		if (m_targetPitch < -89.0f) m_targetPitch = -89.0f;
	}

	void Camera::UpdateKey(float deltaTime, CameraMovement direction)
	{
		float speed = m_speed * deltaTime;
		if (direction == CameraMovement::FORWARD)
			m_targetPosition = m_targetPosition + m_forward * speed;
		else if (direction == CameraMovement::BACKWARD)
			m_targetPosition = m_targetPosition - m_forward * speed;
		else if (direction == CameraMovement::LEFT)
			m_targetPosition = m_targetPosition - m_right * speed;
		else if (direction == CameraMovement::RIGHT)
			m_targetPosition = m_targetPosition + m_right * speed;
		else if (direction == CameraMovement::UP)
			m_targetPosition = m_targetPosition + m_worldUp * speed;
		else if (direction == CameraMovement::DOWN)
			m_targetPosition = m_targetPosition - m_worldUp * speed;
	}

	void Camera::UpdateView()
	{
		m_view = LookAt(m_position, m_position + m_forward, m_up);
		m_viewProjection = m_projection * m_view;
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
		m_fovY = fovY;
		m_aspectRatio = aspectRatio;
		m_nearPlane = nearPlane;
		m_farPlane = farPlane;
		m_projection = glm::perspective(glm::radians(m_fovY), m_aspectRatio, m_nearPlane, m_farPlane);
	}


	Camera::~Camera()
	{
	}

}
