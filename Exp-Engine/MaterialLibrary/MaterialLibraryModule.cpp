//#include <glad/glad.h>



//#include <GLFW/glfw3.h>

#include "MaterialLibraryModule.h"

namespace Exp
{
	void Exp::MaterialLibraryModule::StartUp()
	{
		rmt_ScopedCPUSample(MaterialLibraryModuleStartUp, 0);

		GenerateDefaultMaterials();
		GenerateDefaultShaders();

		spdlog::info("MaterialLibraryModule StartUp");
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

		spdlog::info("MaterialLibraryModule Shutdown");
	}

	void MaterialLibraryModule::PostInitialize()
	{
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

		// Unlit
		Material * UnlitMaterial = new Material(Resources::LoadShader("unlit", "../resources/shaders/unlit.vert", "../resources/shaders/unlit.frag"));
		UnlitMaterial->SetTexture("material.diffuse", Resources::LoadTexture("default_diffuse", "../resources/texture/checkerboard.png", GL_TEXTURE_2D, GL_RGB), 0);
		DefaultMaterials[SID("unlit")] = UnlitMaterial;

		// DebugLight
		Material* DebugLightMaterial = new Material(Resources::LoadShader("debugLight", "../resources/shaders/debug_light.vert", "../resources/shaders/debug_light.frag"));
		DefaultMaterials[SID("debugLight")] = DebugLightMaterial;

		/*Material * DepthMaterial = new Material(Resources::LoadShader("depth", "../resources/shaders/depth.vert", "../resources/shaders/depth.frag"));
		DefaultMaterials[SID("depth")] = DepthMaterial;*/

	}

	void MaterialLibraryModule::GenerateDefaultShaders()
	{
		Shader* DeferredDirectionalLightShader = Resources::LoadShader("deferredDirectional", "../resources/shaders/directional.vert", "../resources/shaders/directional.frag");
		DeferredDirectionalLightShader->use();
		DeferredDirectionalLightShader->setInt("gPosition", 0);
		DeferredDirectionalLightShader->setInt("gNormal", 1);
		DeferredDirectionalLightShader->setInt("gAlbedoSpec", 2);
		DefaultShaders[SID("deferredDirectional")] = DeferredDirectionalLightShader;

		Shader* DeferredPointLightShader = Resources::LoadShader("deferredPoint", "../resources/shaders/point.vert", "../resources/shaders/point.frag");
		DeferredPointLightShader->use();
		DeferredPointLightShader->setInt("gPosition", 0);
		DeferredPointLightShader->setInt("gNormal", 1);
		DeferredPointLightShader->setInt("gAlbedoSpec", 2);
		DefaultShaders[SID("deferredPoint")] = DeferredPointLightShader;

		Shader* StencilLightShader = Resources::LoadShader("stencilLightShader", "../resources/shaders/stencil_light.vert", "../resources/shaders/stencil_light.frag");
		StencilLightShader->use();
		DefaultShaders[SID("stencilLightShader")] = StencilLightShader;
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
			spdlog::error("Material template '{}' requested, but it did not exist", base);
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

	Shader* MaterialLibraryModule::GetShader(std::string name)
	{
		auto found = DefaultShaders.find(SID(name));
		if (found != DefaultShaders.end())
		{
			return DefaultShaders[SID(name)];
		}

		return nullptr;
	}
}