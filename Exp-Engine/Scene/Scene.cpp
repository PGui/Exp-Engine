#include "Scene.h"

namespace Exp
{
	SceneNode* Scene::root = new SceneNode(0);
	unsigned int Scene::counterID = 0;
	// --------------------------------------------------------------------------------------------
	void Scene::Clear()
	{
		Scene::DeleteSceneNode(root);
		Scene::root = new SceneNode(0);
	}
	// --------------------------------------------------------------------------------------------
	SceneNode* Scene::MakeSceneNode()
	{
		SceneNode* node = new SceneNode(Scene::counterID++);

		root->AddChild(node);
		return node;
	}
	// --------------------------------------------------------------------------------------------
	SceneNode* Scene::MakeSceneNode(Mesh* mesh, Material* material)
	{
		SceneNode* node = new SceneNode(Scene::counterID++);

		node->mesh = mesh;
		node->material = material;

		root->AddChild(node);
		return node;
	}
	// --------------------------------------------------------------------------------------------
	SceneNode* Scene::MakeSceneNode(SceneNode* node)
	{
		SceneNode *newNode = new SceneNode(Scene::counterID++);

		newNode->mesh = node->mesh;
		newNode->material = node->material;
		newNode->boxMin = node->boxMin;
		newNode->boxMax = node->boxMax;

		std::stack<SceneNode*> nodeStack;
		for (unsigned int i = 0; i < node->GetChildCount(); ++i)
			nodeStack.push(node->GetChildByIndex(i));
		while (!nodeStack.empty())
		{
			SceneNode* child = nodeStack.top();
			nodeStack.pop();
			// similarly, create SceneNode for each child and push to scene node memory list.
			SceneNode* newChild = new SceneNode(Scene::counterID++);
			newChild->mesh = child->mesh;
			newChild->material = child->material;
			newChild->boxMin = child->boxMin;
			newChild->boxMax = child->boxMax;
			newNode->AddChild(newChild);

			for (unsigned int i = 0; i < child->GetChildCount(); ++i)
				nodeStack.push(child->GetChildByIndex(i));
		}

		root->AddChild(newNode);
		return newNode;
	}
	// --------------------------------------------------------------------------------------------
	void Scene::DeleteSceneNode(SceneNode *node)
	{
		if (node->GetParent())
		{
			node->GetParent()->RemoveChild(node->GetID());
		}

		delete node;
	}
}
