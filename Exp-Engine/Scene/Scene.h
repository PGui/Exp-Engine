#pragma once
#include <vector>
#include <stack>

#include "SceneNode.h"

#include "../Mesh/Mesh.h"
#include "../Rendering/Material.h"

namespace Exp
{
	class Mesh;
	class Material;
	class SceneNode;

	/*

		The global scene object. Always holds a root node to which other scene nodes can be attached.
		There will always be one global scene object, which can be cleared, configured at will.

	*/
	class Scene
	{
	public:
		static SceneNode* Root;
		static unsigned int CounterID;
	public:

		static void Clear();

		static SceneNode* MakeSceneNode();
		static SceneNode* MakeSceneNode(Mesh* mesh, Material* material);
		static SceneNode* MakeSceneNode(SceneNode* node);

		static void DeleteSceneNode(SceneNode* node);
	};
}