#pragma once
#include "SceneNode.h"
#include "Scene.h"

#include "../Glm/glm.hpp"
#include "../Mesh/Cube.h"

#include "../Resources/Resources.h"
#include "../Rendering/Material.h"

namespace Exp
{
	class TextureCube;
	class Material;
	class Shader;
	class Renderer;
	class Cube;

	class Skybox:
		public SceneNode
	{
	private:
		TextureCube * cubeMap = nullptr;
		Shader * shader = nullptr;

	public:
		Skybox();
		~Skybox();

		void SetCubemap(TextureCube* cubemap);
		TextureCube* GetCubemap();
	};


}
