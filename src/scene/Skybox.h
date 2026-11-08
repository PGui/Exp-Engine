#pragma once
#include "SceneNode.h"
#include "Scene.h"

#include <glm/glm.hpp>
#include "../mesh/Cube.h"

#include "../resources/Resources.h"
#include "../rendering/Material.h"

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
