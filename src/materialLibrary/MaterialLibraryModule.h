#pragma once

//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
#include <vector>
#include <map>

#include "../hash/Hash.h"
#include "../Profiling/RemoteryModule.h"
#include "../Rendering/Material.h"
#include "../Resources/Resources.h"
#include "../Module/ModuleInterface.h"


namespace Exp
{
	class MaterialLibraryModule :
		public IModuleInterface
	{
	public:
		virtual void StartUp();
		virtual void Shutdown();
		virtual void PostInitialize();
	private:
		//Internal Default materials that will be copied to create instances.
		std::map<unsigned, Material*> defaultMaterials;
		std::map<unsigned, Shader*> defaultShaders;

		//Created materials from default materials or added materials.
		std::vector<Material*> materials;

	public:
		MaterialLibraryModule();
		virtual ~MaterialLibraryModule();

		Material * CreateMaterial(std::string base = "default");

		Material * GetMaterial(std::string name);

		Shader* GetShader(std::string name);


	private:
		void GenerateDefaultMaterials();
		void GenerateDefaultShaders();
	};
}