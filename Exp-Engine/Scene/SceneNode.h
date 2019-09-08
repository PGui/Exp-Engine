#pragma once

#include "../Mesh/Mesh.h"
#include "../Rendering/Material.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

#include <vector>

namespace Exp
{
	class SceneNode
	{
	public:
		Mesh * Mesh;
		Material * Material;

		// bounding box 
		glm::vec3 BoxMin = glm::vec3(-99999.0f);
		glm::vec3 BoxMax = glm::vec3(99999.0f);

	public:
		SceneNode(unsigned int id);
		~SceneNode();

	private:
		std::vector<SceneNode*> Children;
		SceneNode * Parent = nullptr;

		glm::mat4 Transform = glm::mat4(1.0f);
		glm::mat4 PreviousTransform = glm::mat4(1.0f);
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);
		glm::vec4 Rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); //Axis + Angle



		bool bDirty = false;

		unsigned int ID;

	public:


		//Transform
		void SetPosition(glm::vec3 position);
		void SetRotation(glm::vec4 rotation);
		void SetScale(glm::vec3 scale);

		glm::vec3 GetLocalPosition();
		glm::vec3 GetLocalScale();
		glm::vec4 GetRotation();
		glm::vec3 GetWorldPosition();
		glm::vec3 GetWorldScale();

		const glm::mat4 & GetTransform();

		void UpdateTransform(bool updatePrevTransform = false);


		//Node management
		const unsigned int GetID() const;
		void AddChild(SceneNode * node);
		void RemoveChild(const unsigned int id);
		const std::vector<SceneNode*> & GetChildren() const;
		unsigned int            GetChildCount();
		SceneNode              *GetChild(unsigned int id);
		SceneNode              *GetChildByIndex(unsigned int index);
		SceneNode              *GetParent();
	};
}