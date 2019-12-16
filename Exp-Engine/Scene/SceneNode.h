#pragma once
#include <vector>
#include <algorithm>
#include "../Mesh/Mesh.h"
#include "../Rendering/Material.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"



namespace Exp
{
	class SceneNode
	{
	public:
		Mesh * mesh;
		Material * material;

		// bounding box 
		glm::vec3 boxMin = glm::vec3(-99999.0f);
		glm::vec3 boxMax = glm::vec3(99999.0f);

	public:
		SceneNode(unsigned int id);
		~SceneNode();

	private:
		std::vector<SceneNode*> children;
		SceneNode * parent = nullptr;

		glm::mat4 transform = glm::mat4(1.0f);
		glm::mat4 previousTransform = glm::mat4(1.0f);
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);
		glm::vec4 rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); //Axis + Angle



		bool dirty = false;

		unsigned int id;

	public:


		//transform
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