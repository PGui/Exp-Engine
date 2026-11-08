#include "SceneNode.h"

namespace Exp
{

	SceneNode::SceneNode(unsigned int id) :
		id(id)
	{
	}


	SceneNode::~SceneNode()
	{
		// A node owns its children (but not the Mesh/Material it references).
		for (SceneNode* child : children)
		{
			child->parent = nullptr;
			delete child;
		}
		children.clear();

		if (parent)
		{
			parent->RemoveChild(this);
		}
	}

	void SceneNode::SetPosition(glm::vec3 position)
	{
		this->position = position;
		dirty = true;
	}

	void SceneNode::SetRotation(glm::vec4 rotation)
	{
		this->rotation = rotation;
		dirty = true;
	}

	void SceneNode::SetScale(glm::vec3 scale)
	{
		this->scale = scale;
		dirty = true;
	}

	glm::vec3 SceneNode::GetLocalPosition()
	{
		return position;
	}

	glm::vec3 SceneNode::GetLocalScale()
	{
		return scale;
	}

	glm::vec4 SceneNode::GetRotation()
	{
		return rotation;
	}

	glm::vec3 SceneNode::GetWorldPosition()
	{
		return glm::vec3(transform * glm::vec4(position, 1.0f));
	}

	glm::vec3 SceneNode::GetWorldScale()
	{
		return  glm::vec3(glm::abs(transform[0][0]), glm::abs(transform[1][1]), glm::abs(transform[2][2]));
	}

	const glm::mat4 & SceneNode::GetTransform()
	{
		if (dirty)
		{
			UpdateTransform(false);
		}
		return transform;
	}

	void SceneNode::UpdateTransform(bool updatePrevTransform)
	{
		if (updatePrevTransform)
		{
			previousTransform = transform;
		}

		//Current Node
		if (dirty)
		{
			transform = glm::mat4(1.0f);
			// Standard T * R * S order (S before R would shear with non-uniform scales).
			transform = glm::translate(transform, position);
			transform = glm::rotate(transform, glm::radians(rotation.w), glm::vec3(rotation));
			transform = glm::scale(transform, scale);
			if (parent)
			{
				transform = parent->transform * transform;
			}
		}

		//Children
		for (auto & child : children)
		{
			if (dirty)
			{
				child->dirty = true;
			}
			child->UpdateTransform(updatePrevTransform);
		}
		dirty = false;
	}

	const unsigned int SceneNode::GetID() const
	{
		return id;
	}

	void SceneNode::AddChild(SceneNode * node)
	{
		if (node->parent)
		{
			node->parent->RemoveChild(node);
		}
		node->parent = this;
		children.push_back(node);
	}

	void SceneNode::RemoveChild(const unsigned int id)
	{
		RemoveChild(GetChild(id));
	}

	void SceneNode::RemoveChild(SceneNode* node)
	{
		auto it = std::find(children.begin(), children.end(), node);
		if (it != children.end())
		{
			(*it)->parent = nullptr;
			children.erase(it);
		}
	}

	const std::vector<SceneNode*> & SceneNode::GetChildren() const
	{
		return children;
	}

	unsigned int SceneNode::GetChildCount()
	{
		return (unsigned int)children.size();
	}

	SceneNode * SceneNode::GetChild(const unsigned int id)
	{
		std::vector<SceneNode*>::iterator it = std::find_if(children.begin(), children.end(), [id](SceneNode * Node) {return Node->GetID() == id; });
		return (it != children.end()) ? *it : nullptr;
	}

	SceneNode * SceneNode::GetChildByIndex(unsigned int index)
	{
		assert(index < GetChildCount());
		return children[index];
	}

	SceneNode * SceneNode::GetParent()
	{
		return parent;
	}

}
