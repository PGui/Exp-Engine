#include "Skybox.h"

namespace Exp
{
	Skybox::Skybox(): 
		SceneNode(Scene::CounterID++)
	{
		Scene::Root->AddChild(this);

		m_Shader = Resources::LoadShader("background", "../resources/shaders/skybox.vert", "../resources/shaders/skybox.frag");
		Material = new Exp::Material(m_Shader);
		Mesh = new Cube();
		BoxMin = glm::vec3(-99999.0);
		BoxMax = glm::vec3(99999.0);

		// default material configuration
		Material->SetFloat("Exposure", 1.0f);
		Material->DepthCompare = GL_LEQUAL;
		Material->Cull = false;
		Material->ShadowCast = false;
		Material->ShadowReceive = false;
	}

	Skybox::~Skybox()
	{
		//TODO Refacto
		delete Mesh;
		delete Material;
	}

	void Skybox::SetCubemap(TextureCube* cubemap)
	{
		m_CubeMap = cubemap;
		Material->SetTextureCube("background", m_CubeMap, 0);
		Material->SetFloat("lodLevel", 1.5f);
	}
	TextureCube* Skybox::GetCubemap()
	{
		return m_CubeMap;
	}
}