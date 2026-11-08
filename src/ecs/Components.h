#pragma once
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Exp
{
	class SceneNode;

	// Human readable label, shown in the entity inspector.
	struct NameComponent
	{
		std::string value;
	};

	// World transform of an entity. Rotation is stored as Euler angles (degrees) for easy editing.
	struct TransformComponent
	{
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);

		glm::mat4 Matrix() const
		{
			const glm::quat q = glm::quat(glm::radians(rotation));
			return glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(q) * glm::scale(glm::mat4(1.0f), scale);
		}
	};

	// Renders a model hierarchy (owned by Resources / Scene) at the entity transform.
	struct MeshRendererComponent
	{
		SceneNode* model = nullptr;
		bool visible = true;
		bool castShadows = true;
	};

	// Simple behaviour: constant rotation, updated at the fixed simulation rate.
	struct SpinComponent
	{
		glm::vec3 degreesPerSecond = glm::vec3(0.0f, 45.0f, 0.0f);
	};
}
