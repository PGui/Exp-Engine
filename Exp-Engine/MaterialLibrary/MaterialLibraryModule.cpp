#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "MaterialLibraryModule.h"
#include "../hash/Hash.h"
#include "../Profiling/RemoteryModule.h"
#include "../Rendering/Material.h"


namespace Exp
{
	void Exp::MaterialLibraryModule::StartUp()
	{
		rmt_ScopedCPUSample(MaterialLibraryModuleStartUp, 0);

		GenerateDefaultMaterials();

		std::cout << "MaterialLibraryModule StartUp" << std::endl;
	}

	void Exp::MaterialLibraryModule::Shutdown()
	{
		rmt_ScopedCPUSample(MaterialLibraryModuleShutdown, 0);

		for (auto it = DefaultMaterials.begin(); it != DefaultMaterials.end(); ++it)
		{
			delete it->second;
		}
		for (unsigned int i = 0; i < Materials.size(); ++i)
		{
			delete Materials[i];
		}

		std::cout << "MaterialLibraryModule Shutdown" << std::endl;
	}

	MaterialLibraryModule::MaterialLibraryModule()
	{
	}

	MaterialLibraryModule::~MaterialLibraryModule()
	{
		
	}

	void MaterialLibraryModule::GenerateDefaultMaterials()
	{
		//Default
		/*Material * DefaultMaterial = new Material(Resources::LoadShader("default", "../resources/shaders/forward_shading.vert", "../resources/shaders/forward_shading.frag"));
		DefaultMaterial->SetTexture("material.diffuse", Resources::LoadTexture("default_diffuse", "../resources/texture/checkerboard.png", GL_TEXTURE_2D, GL_RGB), 0);
		DefaultMaterial->SetTexture("material.specular", Resources::LoadTexture("default_specular", "../resources/texture/container2_specular.png", GL_TEXTURE_2D, GL_RGB), 1);
		DefaultMaterial->SetFloat("material.shininess", 32.0f);
		DefaultMaterials[SID("default")] = DefaultMaterial;*/

		//Temp default
		Material * DefaultMaterial = new Material(Resources::LoadShader("default", "../resources/shaders/g_buffer.vert", "../resources/shaders/g_buffer.frag"));
		DefaultMaterial->SetTexture("material.diffuse", Resources::LoadTexture("default_diffuse", "../resources/texture/checkerboard.png", GL_TEXTURE_2D, GL_RGB), 0);
		DefaultMaterial->SetTexture("material.specular", Resources::LoadTexture("default_specular", "../resources/texture/container2_specular.png", GL_TEXTURE_2D, GL_RGB), 1);
		DefaultMaterial->SetTexture("material.normals", Resources::LoadTexture("default_normals", "../resources/texture/checkerboard.png", GL_TEXTURE_2D, GL_RGB), 2);
		DefaultMaterials[SID("default")] = DefaultMaterial;

		//Blit Material
		Material *DefaultBlitMaterial = new Material(Resources::LoadShader("blit", "../resources/shaders/screen_quad.vert", "../resources/shaders/default_blit.frag"));
		DefaultBlitMaterial->DepthTest = false;
		DefaultBlitMaterial->DepthWrite = false;
		DefaultBlitMaterial->SetTexture("screenTexture", Resources::LoadTexture("default_diffuse", "../resources/texture/checkerboard.png", GL_TEXTURE_2D, GL_RGB), 0);
		DefaultMaterials[SID("blit")] = DefaultBlitMaterial;

		Material * UnlitMaterial = new Material(Resources::LoadShader("unlit", "../resources/shaders/unlit.vert", "../resources/shaders/unlit.frag"));
		UnlitMaterial->SetTexture("material.diffuse", Resources::LoadTexture("default_diffuse", "../resources/texture/checkerboard.png", GL_TEXTURE_2D, GL_RGB), 0);
		DefaultMaterials[SID("unlit")] = UnlitMaterial;

		/*Material * DepthMaterial = new Material(Resources::LoadShader("depth", "../resources/shaders/depth.vert", "../resources/shaders/depth.frag"));
		DefaultMaterials[SID("depth")] = DepthMaterial;*/

	}

	Material* MaterialLibraryModule::CreateMaterial(std::string base)
	{
		auto found = DefaultMaterials.find(SID(base));
		if (found != DefaultMaterials.end())
		{
			Material copy = *found->second;
			Material* mat = new Material(copy);
			Materials.push_back(mat);
			return mat;
		}
		else
		{
			std::cout << "Material of template: " + base + " requested, but template did not exist." << std::endl;
			return nullptr;
		}
	}

	Material * MaterialLibraryModule::GetMaterial(std::string name)
	{
		auto found = DefaultMaterials.find(SID(name));
		if (found != DefaultMaterials.end())
		{
			return DefaultMaterials[SID(name)];
		}

		return nullptr;
	}
}