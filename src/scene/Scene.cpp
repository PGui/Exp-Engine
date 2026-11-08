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
	// Deep copy of a node hierarchy: structure and local transforms are kept,
	// Mesh/Material pointers are shared (they are owned by Resources/MaterialLibrary).
	static SceneNode* CloneHierarchy(SceneNode* source)
	{
		SceneNode* copy = new SceneNode(Scene::counterID++);
		copy->mesh = source->mesh;
		copy->material = source->material;
		copy->boxMin = source->boxMin;
		copy->boxMax = source->boxMax;
		copy->SetPosition(source->GetLocalPosition());
		copy->SetRotation(source->GetRotation());
		copy->SetScale(source->GetLocalScale());

		for (SceneNode* child : source->GetChildren())
		{
			copy->AddChild(CloneHierarchy(child));
		}
		return copy;
	}

	SceneNode* Scene::MakeSceneNode(SceneNode* node)
	{
		SceneNode* newNode = CloneHierarchy(node);
		root->AddChild(newNode);
		return newNode;
	}
	// --------------------------------------------------------------------------------------------
	void Scene::DeleteSceneNode(SceneNode *node)
	{
		// The destructor detaches the node from its parent and deletes its children.
		delete node;
	}
}
