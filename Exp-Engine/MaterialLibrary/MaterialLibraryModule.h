#pragma once

#include <vector>
#include <map>
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
		//Internal Default Materials that will be copied to create instances.
		std::map<unsigned, Material*> DefaultMaterials;
		std::map<unsigned, Shader*> DefaultShaders;

		//Created materials from default materials or added materials.
		std::vector<Material*> Materials;

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