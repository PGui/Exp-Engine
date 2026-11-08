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

	// Transform at the previous simulation step, used to interpolate rendering
	// between fixed steps. Maintained automatically by the ECSModule.
	struct PreviousTransformComponent
	{
		TransformComponent value;
	};

	// Blend two transforms (position/scale linearly, rotation along the shortest arc).
	inline glm::mat4 InterpolateTransform(const TransformComponent& from, const TransformComponent& to, float t)
	{
		const glm::vec3 position = glm::mix(from.position, to.position, t);
		const glm::vec3 scale = glm::mix(from.scale, to.scale, t);
		const glm::quat rotation = glm::slerp(glm::quat(glm::radians(from.rotation)), glm::quat(glm::radians(to.rotation)), t);
		return glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), scale);
	}

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
