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
		return glm::vec3(Transform * glm::vec4(Position, 1.0f));
	}

	glm::vec3 SceneNode::GetWorldScale()
	{
		return  glm::vec3(glm::abs(Transform[0][0]), glm::abs(Transform[1][1]), glm::abs(Transform[2][2]));
	}

	const glm::mat4 & SceneNode::GetTransform()
	{
		if (bDirty)
		{
			UpdateTransform(false);
		}
		return Transform;
	}

	void SceneNode::UpdateTransform(bool updatePrevTransform)
	{
		if (updatePrevTransform)
		{
			PreviousTransform = Transform;
		}

		//Current Node
		if (bDirty)
		{
			Transform = glm::mat4(1.0f);
			Transform = glm::translate(Transform, Position);
			Transform = glm::scale(Transform, Scale);
			Transform = glm::rotate(Transform, glm::radians(Rotation.w), glm::vec3(Rotation));
			if (Parent)
			{
				Transform = Parent->Transform * Transform;
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
