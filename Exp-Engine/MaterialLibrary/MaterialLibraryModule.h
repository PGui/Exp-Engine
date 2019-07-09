#pragma once
//#include "../shading/Material.h"
//#include "../shading/Shader.h"
//#include "../shading/Texture.h"
#include "../resources/Resources.h"
#include <vector>
#include <map>
#include "../Module/ModuleInterface.h"

namespace Exp
{
	class MaterialLibraryModule :
		public IModuleInterface
	{
	public:
		virtual void StartUp();

		virtual void Shutdown();
	private:
		//Internal Default Materials that will be copied to create instances.
		std::map<unsigned, Material*> DefaultMaterials;

		//Created materials from default materials or added materials.
		std::vector<Material*> Materials;

	public:
		MaterialLibraryModule();
		virtual ~MaterialLibraryModule();

		Material * CreateMaterial(std::string base = "default");

		Material * GetMaterial(std::string name);

	private:
		void GenerateDefaultMaterials();
	};
}