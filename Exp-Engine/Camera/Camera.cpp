#include "Camera.h"

namespace Exp
{
	Camera::Camera(glm::vec3 position, glm::vec3 forward, glm::vec3 up) :
		_position(position),
		_forward(forward),
		_up(up)

	{
		_worldUp = _up;
		_targetPosition = _position;
		_targetYaw = _yaw;
		_targetPitch = _pitch;


		updateView();

		setPerspective(_fovY, _aspectRatio, _nearPlane, _farPlane);
	}

	void Camera::update(float deltatime)
	{
		_position = glm::lerp(_position, _targetPosition, glm::clamp(1.f - pow(1.f - _damping / 60.f, 60.f * deltatime), 0.0f, 1.0f));
		_yaw = glm::lerp(_yaw, _targetYaw, glm::clamp(1.f - pow(1.f - _damping * 2.0f / 60.f, 60.f * deltatime), 0.0f, 1.0f));
		_pitch = glm::lerp(_pitch, _targetPitch, glm::clamp(1.f - pow(1.f - _damping * 2.0f / 60.f, 60.f * deltatime), 0.0f, 1.0f));

		glm::vec3 newForward;
		newForward.x = float(cos(0.0174533 * _pitch) * cos(0.0174533 * _yaw));
		newForward.y = float(sin(0.0174533 * _pitch));
		newForward.z = float(cos(0.0174533 * _pitch) * sin(0.0174533 * _yaw));
		_forward = glm::normalize(newForward);
		_right = glm::normalize(glm::cross(_forward, _worldUp));
		_up = glm::cross(_right, _forward);

		updateView();
	}

	void Camera::updateMouse(float deltaX, float deltaY)
	{
		if (_disableMouse)
			return;

		float x = deltaX * _sensitivity;
		float y = deltaY * _sensitivity;

		_targetYaw += x;
		_targetPitch += y;

		if (_targetYaw == 0.0f) _targetYaw = 0.01f;
		if (_targetPitch == 0.0f) _targetPitch = 0.01f;

		if (_targetPitch > 89.0f)  _targetPitch = 89.0f;
		if (_targetPitch < -89.0f) _targetPitch = -89.0f;
	}

	void Camera::updateKey(float deltaTime, CameraMovement direction)
	{
		float speed = _speed * deltaTime;
		if (direction == CameraMovement::FORWARD)
			_targetPosition = _targetPosition + _forward * speed;
		else if (direction == CameraMovement::BACKWARD)
			_targetPosition = _targetPosition - _forward * speed;
		else if (direction == CameraMovement::LEFT)
			_targetPosition = _targetPosition - _right * speed;
		else if (direction == CameraMovement::RIGHT)
			_targetPosition = _targetPosition + _right * speed;
		else if (direction == CameraMovement::UP)
			_targetPosition = _targetPosition + _worldUp * speed;
		else if (direction == CameraMovement::DOWN)
			_targetPosition = _targetPosition - _worldUp * speed;
	}

	void Camera::updateView()
	{
		_view = lookAt(_position, _position + _forward, _up);
		_viewProjection = _projection * _view;
	}

	glm::mat4 Camera::lookAt(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)
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

	void Camera::setPerspective(float fovY, float aspectRatio, float nearPlane, float farPlane)
	{
		_fovY = fovY;
		_aspectRatio = aspectRatio;
		_nearPlane = nearPlane;
		_farPlane = farPlane;
		_projection = glm::perspective(glm::radians(_fovY), _aspectRatio, _nearPlane, _farPlane);
	}


	Camera::~Camera()
	{
	}

}
