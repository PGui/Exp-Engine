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

		for (auto it = defaultMaterials.begin(); it != defaultMaterials.end(); ++it)
		{
			delete it->second;
		}
		for (unsigned int i = 0; i < materials.size(); ++i)
		{
			delete materials[i];
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
		defaultMaterials[SID("default")] = DefaultMaterial;*/

		//Temp default
		Material * DefaultMaterial = new Material(Resources::LoadShader("default", "../resources/shaders/g_buffer.vert", "../resources/shaders/g_buffer.frag"));
		DefaultMaterial->SetTexture("material.diffuse", Resources::LoadTexture("default_diffuse", "../resources/texture/checkerboard.png", GL_TEXTURE_2D, GL_RGB), 0);
		DefaultMaterial->SetTexture("material.specular", Resources::LoadTexture("default_specular", "../resources/texture/container2_specular.png", GL_TEXTURE_2D, GL_RGB), 1);
		DefaultMaterial->SetTexture("material.normals", Resources::LoadTexture("default_normals", "../resources/texture/checkerboard.png", GL_TEXTURE_2D, GL_RGB), 2);
		defaultMaterials[SID("default")] = DefaultMaterial;

		//Blit Material
		Material *DefaultBlitMaterial = new Material(Resources::LoadShader("blit", "../resources/shaders/screen_quad.vert", "../resources/shaders/default_blit.frag"));
		DefaultBlitMaterial->depthTest = false;
		DefaultBlitMaterial->depthWrite = false;
		DefaultBlitMaterial->SetTexture("screenTexture", Resources::LoadTexture("default_diffuse", "../resources/texture/checkerboard.png", GL_TEXTURE_2D, GL_RGB), 0);
		defaultMaterials[SID("blit")] = DefaultBlitMaterial;

		// Unlit
		Material * UnlitMaterial = new Material(Resources::LoadShader("unlit", "../resources/shaders/unlit.vert", "../resources/shaders/unlit.frag"));
		UnlitMaterial->SetTexture("material.diffuse", Resources::LoadTexture("default_diffuse", "../resources/texture/checkerboard.png", GL_TEXTURE_2D, GL_RGB), 0);
		defaultMaterials[SID("unlit")] = UnlitMaterial;

		// DebugLight
		Material* DebugLightMaterial = new Material(Resources::LoadShader("debugLight", "../resources/shaders/debug_light.vert", "../resources/shaders/debug_light.frag"));
		defaultMaterials[SID("debugLight")] = DebugLightMaterial;

		/*Material * DepthMaterial = new Material(Resources::LoadShader("depth", "../resources/shaders/depth.vert", "../resources/shaders/depth.frag"));
		defaultMaterials[SID("depth")] = DepthMaterial;*/

	}

	void MaterialLibraryModule::GenerateDefaultShaders()
	{
		{
			Shader* deferredDirectionalLightShader = Resources::LoadShader(
				"deferredDirectional", 
				"../resources/shaders/directional.vert", 
				"../resources/shaders/directional.frag");
			deferredDirectionalLightShader->Use();
			deferredDirectionalLightShader->SetInt("gPosition", 0);
			deferredDirectionalLightShader->SetInt("gNormal", 1);
			deferredDirectionalLightShader->SetInt("gAlbedoSpec", 2);
			defaultShaders[SID("deferredDirectional")] = deferredDirectionalLightShader;
		}

		{
			Shader* deferredPointLightShader = Resources::LoadShader(
				"deferredPoint", 
				"../resources/shaders/point.vert", 
				"../resources/shaders/point.frag");
			deferredPointLightShader->Use();
			deferredPointLightShader->SetInt("gPosition", 0);
			deferredPointLightShader->SetInt("gNormal", 1);
			deferredPointLightShader->SetInt("gAlbedoSpec", 2);
			defaultShaders[SID("deferredPoint")] = deferredPointLightShader;
		}
		// global post-processing shader
		{
			Shader * postProcessShader = Resources::LoadShader(
				"postprocess", 
				"../resources/shaders/screen_quad.vert", 
				"../resources/shaders/post_processing.frag");
			postProcessShader->Use();
			/*postProcessShader->SetInt("TexSrc", 0);
			postProcessShader->SetInt("TexBloom1", 1);
			postProcessShader->SetInt("TexBloom2", 2);
			postProcessShader->SetInt("TexBloom3", 3);
			postProcessShader->SetInt("TexBloom4", 4);
			postProcessShader->SetInt("gMotion", 5);*/
			defaultShaders[SID("postprocess")] = postProcessShader;
		}

		{
			Shader* stencilLightShader = Resources::LoadShader(
				"stencilLightShader", 
				"../resources/shaders/stencil_light.vert", 
				"../resources/shaders/stencil_light.frag");
			stencilLightShader->Use();
			defaultShaders[SID("stencilLightShader")] = stencilLightShader;
		}
	}

	Material* MaterialLibraryModule::CreateMaterial(std::string base)
	{
		auto found = defaultMaterials.find(SID(base));
		if (found != defaultMaterials.end())
		{
			Material copy = *found->second;
			Material* mat = new Material(copy);
			materials.push_back(mat);
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
		auto found = defaultMaterials.find(SID(name));
		if (found != defaultMaterials.end())
		{
			return defaultMaterials[SID(name)];
		}

		return nullptr;
	}

	Shader* MaterialLibraryModule::GetShader(std::string name)
	{
		auto found = defaultShaders.find(SID(name));
		if (found != defaultShaders.end())
		{
			return defaultShaders[SID(name)];
		}

		return nullptr;
	}
}