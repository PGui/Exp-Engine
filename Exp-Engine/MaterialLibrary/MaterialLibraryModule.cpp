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
		DefaultMaterial->SetTexture("material.diffuse", Resources::LoadTexture("default_diffuse", "../resources/textures/checkerboard.png", GL_TEXTURE_2D, GL_RGB), 0);
		DefaultMaterial->SetTexture("material.specular", Resources::LoadTexture("default_specular", "../resources/textures/container2_specular.png", GL_TEXTURE_2D, GL_RGB), 1);
		DefaultMaterial->SetFloat("material.shininess", 32.0f);
		defaultMaterials[SID("default")] = DefaultMaterial;*/

		/*Shader* defaultShader = Resources::LoadShader("default", "shaders/deferred/g_buffer.vs", "shaders/deferred/g_buffer.fs");
		Material* defaultMat = new Material(defaultShader);
		defaultMat->Type = MATERIAL_DEFAULT;
		defaultMat->SetTexture("TexAlbedo", Resources::LoadTexture("default albedo", "textures/checkerboard.png", GL_TEXTURE_2D, GL_RGB), 3);
		defaultMat->SetTexture("TexNormal", Resources::LoadTexture("default normal", "textures/norm.png"), 4);
		defaultMat->SetTexture("TexMetallic", Resources::LoadTexture("default metallic", "textures/black.png"), 5);
		defaultMat->SetTexture("TexRoughness", Resources::LoadTexture("default roughness", "textures/checkerboard.png"), 6);
		m_DefaultMaterials[SID("default")] = defaultMat;*/

		//Temp default
		Material * DefaultMaterial = new Material(Resources::LoadShader("default", "../resources/shaders/g_buffer.vert", "../resources/shaders/g_buffer.frag"));
		//DefaultMaterial->type = MaterialType::MATERIAL_DEFAULT;
		DefaultMaterial->SetTexture("TexAlbedo", Resources::LoadTexture("default_albedo", "../resources/textures/pbr/default/checkerboard.png", GL_TEXTURE_2D, GL_RGB), 3);
		DefaultMaterial->SetTexture("TexNormal", Resources::LoadTexture("default_normal", "../resources/textures/pbr/default/norm.png"), 4);
		DefaultMaterial->SetTexture("TexMetallic", Resources::LoadTexture("default_metallic", "../resources/textures/pbr/default/black.png"), 5);
		DefaultMaterial->SetTexture("TexRoughness", Resources::LoadTexture("default_roughness", "../resources/textures/pbr/default/black.png"), 6);
		defaultMaterials[SID("default")] = DefaultMaterial;

		//Blit Material
		Material *DefaultBlitMaterial = new Material(Resources::LoadShader("blit", "../resources/shaders/screen_quad.vert", "../resources/shaders/default_blit.frag"));
		DefaultBlitMaterial->depthTest = false;
		DefaultBlitMaterial->depthWrite = false;
		DefaultBlitMaterial->SetTexture("screenTexture", Resources::LoadTexture("default_diffuse", "../resources/textures/checkerboard.png", GL_TEXTURE_2D, GL_RGB), 0);
		defaultMaterials[SID("blit")] = DefaultBlitMaterial;

		// Unlit
		Material * UnlitMaterial = new Material(Resources::LoadShader("unlit", "../resources/shaders/unlit.vert", "../resources/shaders/unlit.frag"));
		UnlitMaterial->SetTexture("material.diffuse", Resources::LoadTexture("default_diffuse", "../resources/textures/checkerboard.png", GL_TEXTURE_2D, GL_RGB), 0);
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
			//deferredAmbientShader = Cell::Resources::LoadShader("deferred ambient", "shaders/deferred/screen_ambient.vs", "shaders/deferred/ambient.fs");
			Shader* deferredAmbientShader = Resources::LoadShader(
				"deferredAmbientShader",
				"../resources/shaders/screen_ambient.vert",
				"../resources/shaders/ambient.frag");
			deferredAmbientShader->Use();
			deferredAmbientShader->SetInt("gPositionMetallic", 0);
			deferredAmbientShader->SetInt("gNormalRoughness", 1);
			deferredAmbientShader->SetInt("gAlbedoAO", 2);
			deferredAmbientShader->SetInt("envIrradiance", 3);
			deferredAmbientShader->SetInt("envPrefilter", 4);
			deferredAmbientShader->SetInt("BRDFLUT", 5);
			deferredAmbientShader->SetInt("TexSSAO", 6);
			defaultShaders[SID("deferredAmbientShader")] = deferredAmbientShader;
		}

		{
			Shader* deferredDirectionalLightShader = Resources::LoadShader(
				"deferredDirectional", 
				"../resources/shaders/directional.vert", 
				"../resources/shaders/directional.frag");
			deferredDirectionalLightShader->Use();
			deferredDirectionalLightShader->SetInt("gPositionMetallic", 0);
			deferredDirectionalLightShader->SetInt("gNormalRoughness", 1);
			deferredDirectionalLightShader->SetInt("gAlbedoAO", 2);
			defaultShaders[SID("deferredDirectional")] = deferredDirectionalLightShader;
		}

		{
			Shader* deferredPointLightShader = Resources::LoadShader(
				"deferredPoint", 
				"../resources/shaders/point.vert", 
				"../resources/shaders/point.frag");
			deferredPointLightShader->Use();
			deferredPointLightShader->SetInt("gPositionMetallic", 0);
			deferredPointLightShader->SetInt("gNormalRoughness", 1);
			deferredPointLightShader->SetInt("gAlbedoAO", 2);
			defaultShaders[SID("deferredPoint")] = deferredPointLightShader;
		}
		// global post-processing shader
		{
			Shader * postProcessShader = Resources::LoadShader(
				"postprocess", 
				"../resources/shaders/screen_quad.vert", 
				"../resources/shaders/post_processing.frag");
			postProcessShader->Use();
			postProcessShader->SetInt("TexSrc", 0);
			/*postProcessShader->SetInt("TexBloom1", 1);
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