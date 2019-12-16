#pragma once
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <spdlog/spdlog.h>
#include "../glm/glm.hpp"

namespace Exp
{
	class Renderer;
	class SceneNode;
	class Mesh;
	class Material;

	class ModelLoader
	{
	private:
		static std::vector<Mesh*> meshes;

	public:
		static void       Clean();
		static SceneNode* LoadMesh(Renderer* renderer, std::string path, bool setDefaultMaterial = true);
	private:
		static SceneNode* ProcessNode(Renderer* renderer, aiNode* aNode, const aiScene* aScene, std::string directory, bool setDefaultMaterial);
		static Mesh*      ParseMesh(aiMesh* aMesh, const aiScene* aScene, glm::vec3& out_Min, glm::vec3& out_Max);
		static Material*  ParseMaterial(Renderer* renderer, aiMaterial* aMaterial, const aiScene* aScene, std::string directory);

		static std::string ProcessPath(aiString* path, std::string directory);
	};
}
