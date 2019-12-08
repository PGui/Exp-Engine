#include "SceneNode.h"

namespace Exp
{

	SceneNode::SceneNode(unsigned int id) :
		ID(id)
	{
	}


	SceneNode::~SceneNode()
	{
	}

	void SceneNode::SetPosition(glm::vec3 position)
	{
		Position = position;
		bDirty = true;
	}

	void SceneNode::SetRotation(glm::vec4 rotation)
	{
		Rotation = rotation;
		bDirty = true;
	}

	void SceneNode::SetScale(glm::vec3 scale)
	{
		Scale = scale;
		bDirty = true;
	}

	glm::vec3 SceneNode::GetLocalPosition()
	{
		return Position;
	}

	glm::vec3 SceneNode::GetLocalScale()
	{
		return Scale;
	}

	glm::vec4 SceneNode::GetRotation()
	{
		return Rotation;
	}

	glm::vec3 SceneNode::GetWorldPosition()
	{
		return glm::vec3(transform * glm::vec4(Position, 1.0f));
	}

	glm::vec3 SceneNode::GetWorldScale()
	{
		return  glm::vec3(glm::abs(transform[0][0]), glm::abs(transform[1][1]), glm::abs(transform[2][2]));
	}

	const glm::mat4 & SceneNode::GetTransform()
	{
		if (bDirty)
		{
			UpdateTransform(false);
		}
		return transform;
	}

	void SceneNode::UpdateTransform(bool updatePrevTransform)
	{
		if (updatePrevTransform)
		{
			PreviousTransform = transform;
		}

		//Current Node
		if (bDirty)
		{
			transform = glm::mat4(1.0f);
			transform = glm::translate(transform, Position);
			transform = glm::scale(transform, Scale);
			transform = glm::rotate(transform, glm::radians(Rotation.w), glm::vec3(Rotation));
			if (Parent)
			{
				transform = Parent->transform * transform;
			}
		}

		//Children
		for (auto & Child : Children)
		{
			if (bDirty)
			{
				Child->bDirty = true;
			}
			Child->UpdateTransform(updatePrevTransform);
		}
		bDirty = false;
	}

	const unsigned int SceneNode::GetID() const
	{
		return ID;
	}

	void SceneNode::AddChild(SceneNode * node)
	{
		if (node->Parent)
		{
			node->Parent->RemoveChild(node->ID);
		}
		node->Parent = this;
		Children.push_back(node);
	}

	void SceneNode::RemoveChild(const unsigned int id)
	{
		auto it = std::find(Children.begin(), Children.end(), GetChild(id));
		if (it != Children.end())
			Children.erase(it);
	}

	const std::vector<SceneNode*> & SceneNode::GetChildren() const
	{
		return Children;
	}

	unsigned int SceneNode::GetChildCount()
	{
		return (unsigned int)Children.size();
	}

	SceneNode * SceneNode::GetChild(const unsigned int id)
	{
		std::vector<SceneNode*>::iterator it = std::find_if(Children.begin(), Children.end(), [id](SceneNode * Node) {return Node->GetID() == id; });
		return (it != Children.end()) ? *it : nullptr;
	}

	SceneNode * SceneNode::GetChildByIndex(unsigned int index)
	{
		assert(index < GetChildCount());
		return Children[index];
	}

	SceneNode * SceneNode::GetParent()
	{
		return Parent;
	}

}
