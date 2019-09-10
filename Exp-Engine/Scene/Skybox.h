#pragma once
#include "SceneNode.h"

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
		TextureCube* m_CubeMap;
		Shader* m_Shader;

	public:
		Skybox();
		~Skybox();

		void SetCubemap(TextureCube* cubemap);
		TextureCube* GetCubemap();
	};


}
