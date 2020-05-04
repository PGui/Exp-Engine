#include "Skybox.h"

namespace Exp
{
	Skybox::Skybox(): 
		SceneNode(Scene::counterID++)
	{
		Scene::root->AddChild(this);

		shader = Resources::LoadShader("background", "../resources/shaders/skybox.vert", "../resources/shaders/skybox.frag");
		material = new Exp::Material(shader);
		mesh = new Cube();
		boxMin = glm::vec3(-99999.0);
		boxMax = glm::vec3(99999.0);

		// default material configuration
		material->SetFloat("Exposure", 1.0f);
		material->depthCompare = GL_LEQUAL;
		material->cull = false;
		material->shadowCast = false;
		material->shadowReceive = false;
	}

	Skybox::~Skybox()
	{
		//TODO Refacto
		delete mesh;
		delete material;
	}

	void Skybox::SetCubemap(TextureCube* cubemap)
	{
		cubeMap = cubemap;
		material->SetTextureCube("background", cubeMap, 0);
		material->SetFloat("lodLevel", 1.5f);
	}
	TextureCube* Skybox::GetCubemap()
	{
		return cubeMap;
	}
}