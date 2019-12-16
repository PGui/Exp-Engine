#include "ModelLoader.h"
#include "../scene/SceneNode.h"
//#include "../rendering/renderer.h"
#include "../Module/ModuleManager.h"
#include "../MaterialLibrary/MaterialLibraryModule.h"
#include "../Mesh/Mesh.h"
#include "../Rendering/Material.h"


namespace Exp
{
	std::vector<Mesh*> ModelLoader::meshes = std::vector<Mesh*>();

	void ModelLoader::Clean()
	{
		for (unsigned int i = 0; i < ModelLoader::meshes.size(); ++i)
		{
			delete ModelLoader::meshes[i];
		}
	}

	SceneNode * ModelLoader::LoadMesh(Renderer * renderer, std::string path, bool setDefaultMaterial)
	{
		spdlog::info("Loading Mesh file at {}", path);

		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			spdlog::error("Assimp failed to load model at {}", path);
			return nullptr;
		}

		std::string directory = path.substr(0, path.find_last_of("/"));

		spdlog::info("Succesfully loaded {}", path);

		return ModelLoader::ProcessNode(renderer, scene->mRootNode, scene, directory, setDefaultMaterial);
	}

	SceneNode * ModelLoader::ProcessNode(Renderer * renderer, aiNode * aNode, const aiScene * aScene, std::string directory, bool setDefaultMaterial)
	{
		SceneNode* node = new SceneNode(0);

		for (unsigned int i = 0; i < aNode->mNumMeshes; ++i)
		{
			glm::vec3 boxMin, boxMax;
			aiMesh*     assimpMesh = aScene->mMeshes[aNode->mMeshes[i]];
			aiMaterial* assimpMat = aScene->mMaterials[assimpMesh->mMaterialIndex];
			Mesh*       mesh = ModelLoader::ParseMesh(assimpMesh, aScene, boxMin, boxMax);
			Material*   material = nullptr;
			if (setDefaultMaterial)
			{
				material = ModelLoader::ParseMaterial(renderer, assimpMat, aScene, directory);
			}

			// if we only have one Mesh, this node itself contains the Mesh/material.
			if (aNode->mNumMeshes == 1)
			{
				node->mesh = mesh;
				if (setDefaultMaterial)
				{
					node->material = material;
				}
				node->boxMin = boxMin;
				node->boxMax = boxMax;
			}
			// otherwise, the meshes are considered on equal depth of its children
			else
			{
				SceneNode* child = new SceneNode(0);
				child->mesh = mesh;
				child->material = material;
				child->boxMin = boxMin;
				child->boxMax = boxMax;
				node->AddChild(child);
			}
		}

		// also recursively parse this node's children 
		for (unsigned int i = 0; i < aNode->mNumChildren; ++i)
		{
			node->AddChild(ModelLoader::ProcessNode(renderer, aNode->mChildren[i], aScene, directory, setDefaultMaterial));
		}

		return node;
	}

	Mesh * ModelLoader::ParseMesh(aiMesh * aMesh, const aiScene * aScene, glm::vec3 & out_Min, glm::vec3 & out_Max)
	{
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;
		std::vector<unsigned int> indices;

		positions.resize(aMesh->mNumVertices);
		normals.resize(aMesh->mNumVertices);
		if (aMesh->mNumUVComponents > 0)
		{
			uv.resize(aMesh->mNumVertices);
			tangents.resize(aMesh->mNumVertices);
			bitangents.resize(aMesh->mNumVertices);
		}
		// we assume a constant of 3 vertex indices per face as we always triangulate in Assimp's
		// post-processing step; otherwise you'll want transform this to a more  flexible scheme.
		indices.resize(aMesh->mNumFaces * (unsigned int)3);

		// store min/max point in local coordinates for calculating approximate bounding box.
		glm::vec3 pMin(99999.0);
		glm::vec3 pMax(-99999.0);

		for (unsigned int i = 0; i < aMesh->mNumVertices; ++i)
		{
			positions[i] = glm::vec3(aMesh->mVertices[i].x, aMesh->mVertices[i].y, aMesh->mVertices[i].z);
			normals[i] = glm::vec3(aMesh->mNormals[i].x, aMesh->mNormals[i].y, aMesh->mNormals[i].z);
			if (aMesh->mTextureCoords[0])
			{
				uv[i] = glm::vec2(aMesh->mTextureCoords[0][i].x, aMesh->mTextureCoords[0][i].y);

			}
			if (aMesh->mTangents)
			{
				tangents[i] = glm::vec3(aMesh->mTangents[i].x, aMesh->mTangents[i].y, aMesh->mTangents[i].z);
				bitangents[i] = glm::vec3(aMesh->mBitangents[i].x, aMesh->mBitangents[i].y, aMesh->mBitangents[i].z);
			}
			if (positions[i].x < pMin.x) pMin.x = positions[i].x;
			if (positions[i].y < pMin.y) pMin.y = positions[i].y;
			if (positions[i].z < pMin.z) pMin.z = positions[i].z;
			if (positions[i].x > pMax.x) pMax.x = positions[i].x;
			if (positions[i].y > pMax.y) pMax.y = positions[i].y;
			if (positions[i].z > pMax.z) pMax.z = positions[i].z;
		}
		for (unsigned int f = 0; f < aMesh->mNumFaces; ++f)
		{
			// we know we're always working with triangles due to TRIANGULATE option.
			for (unsigned int i = 0; i < 3; ++i)
			{
				indices[f * 3 + i] = aMesh->mFaces[f].mIndices[i];
			}
		}

		Mesh *mesh = new Mesh;
		mesh->positions = positions;
		mesh->uv = uv;
		mesh->normals = normals;
		mesh->tangents = tangents;
		mesh->bitangents = bitangents;
		mesh->indices = indices;
		mesh->topology = Mesh::TRIANGLES;
		mesh->Finalize(true);

		out_Min.x = pMin.x;
		out_Min.y = pMin.y;
		out_Min.z = pMin.z;
		out_Max.x = pMax.x;
		out_Max.y = pMax.y;
		out_Max.z = pMax.z;

		// store newly generated Mesh in globally stored Mesh store for memory de-allocation when 
		// a clean is required.
		ModelLoader::meshes.push_back(mesh);

		return mesh;
	}

	Material * ModelLoader::ParseMaterial(Renderer * renderer, aiMaterial * aMaterial, const aiScene * aScene, std::string directory)
	{
		MaterialLibraryModule * MaterialLibrary = Exp::ModuleManager::Get().GetModule<Exp::MaterialLibraryModule>("MaterialLibrary");
		Material* material;
		// check if diffuse texture has alpha, if so: make alpha blend material; 
		aiString file;
		aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		std::string diffPath = std::string(file.C_Str());
		bool alpha = false;
		if (diffPath.find("_alpha") != std::string::npos)
		{
			alpha = true;
		}

		material = MaterialLibrary->CreateMaterial();

		if (aMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			// we only load the first of the list of diffuse textures, we don't really care about 
			// meshes with multiple diffuse layers; same holds for other texture types.
			aiString file;
			aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
			std::string fileName = ModelLoader::ProcessPath(&file, directory);
			// we name the texture the same as the filename as to reduce naming conflicts while 
			// still only loading unique textures.
			Texture* texture = Resources::LoadTexture(fileName, fileName, GL_TEXTURE_2D, alpha ? GL_RGBA : GL_RGB, false /*TODO put this on*/);
			if (texture)
			{
				material->SetTexture("material.diffuse", texture, 0);
			}
		}

		if (aMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
		{
			aiString file;
			aMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
			std::string fileName = ModelLoader::ProcessPath(&file, directory);

			Texture* texture = Resources::LoadTexture(fileName, fileName);
			if (texture)
			{
				material->SetTexture("material.specular", texture, 1);
			}
		}

		if (aMaterial->GetTextureCount(aiTextureType_HEIGHT) > 0)
		{
			aiString file;
			aMaterial->GetTexture(aiTextureType_HEIGHT, 0, &file);
			std::string fileName = ModelLoader::ProcessPath(&file, directory);

			Texture* texture = Resources::LoadTexture(fileName, fileName);
			if (texture)
			{
				material->SetTexture("material.normals", texture, 2);
			}
		}

		return material;



		//Material* material;
		//// check if diffuse texture has alpha, if so: make alpha blend material; 
		//aiString file;
		//aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		//std::string diffPath = std::string(file.C_Str());
		//bool alpha = false;
		//if (diffPath.find("_alpha") != std::string::npos)
		//{
		//	material = renderer->CreateMaterial("alpha discard");
		//	alpha = true;
		//}
		//else  // else, make default deferred material
		//{
		//	material = renderer->CreateMaterial();
		//}

		///* NOTE(Joey):

		//  About texture types:

		//  We use a PBR metallic/roughness workflow so the loaded models are expected to have
		//  textures conform the workflow: albedo, (normal), metallic, roughness, (ao). Since Assimp
		//  made certain assumptions regarding possible types of loaded textures it doesn't directly
		//  translate to our model thus we make some assumptions as well which the 3D author has to
		//  comply with if he wants the Mesh(es) to directly render with its specified textures:

		//	- aiTextureType_DIFFUSE:   Albedo
		//	- aiTextureType_NORMALS:   Normal
		//	- aiTextureType_SPECULAR:  metallic
		//	- aiTextureType_SHININESS: roughness
		//	- aiTextureType_AMBIENT:   AO (ambient occlusion)
		//	- aiTextureType_EMISSIVE:  Emissive

		//*/
		//if (aMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		//{
		//	// we only load the first of the list of diffuse textures, we don't really care about 
		//	// meshes with multiple diffuse layers; same holds for other texture types.
		//	aiString file;
		//	aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		//	std::string fileName = MeshLoader::processPath(&file, directory);
		//	// we name the texture the same as the filename as to reduce naming conflicts while 
		//	// still only loading unique textures.
		//	Texture* texture = Resources::LoadTexture(fileName, fileName, GL_TEXTURE_2D, alpha ? GL_RGBA : GL_RGB, true);
		//	if (texture)
		//	{
		//		material->SetTexture("TexAlbedo", texture, 3);
		//	}
		//}
		//if (aMaterial->GetTextureCount(aiTextureType_DISPLACEMENT) > 0)
		//{
		//	aiString file;
		//	aMaterial->GetTexture(aiTextureType_DISPLACEMENT, 0, &file);
		//	std::string fileName = MeshLoader::processPath(&file, directory);

		//	Texture* texture = Resources::LoadTexture(fileName, fileName);
		//	if (texture)
		//	{
		//		material->SetTexture("TexNormal", texture, 4);
		//	}
		//}
		//if (aMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
		//{
		//	aiString file;
		//	aMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		//	std::string fileName = MeshLoader::processPath(&file, directory);

		//	Texture* texture = Resources::LoadTexture(fileName, fileName);
		//	if (texture)
		//	{
		//		material->SetTexture("TexMetallic", texture, 5);
		//	}
		//}
		//if (aMaterial->GetTextureCount(aiTextureType_SHININESS) > 0)
		//{
		//	aiString file;
		//	aMaterial->GetTexture(aiTextureType_SHININESS, 0, &file);
		//	std::string fileName = MeshLoader::processPath(&file, directory);

		//	Texture* texture = Resources::LoadTexture(fileName, fileName);
		//	if (texture)
		//	{
		//		material->SetTexture("TexRoughness", texture, 6);
		//	}
		//}
		//if (aMaterial->GetTextureCount(aiTextureType_AMBIENT) > 0)
		//{
		//	aiString file;
		//	aMaterial->GetTexture(aiTextureType_AMBIENT, 0, &file);
		//	std::string fileName = MeshLoader::processPath(&file, directory);

		//	Texture* texture = Resources::LoadTexture(fileName, fileName);
		//	if (texture)
		//	{
		//		material->SetTexture("TexAO", texture, 7);
		//	}
		//}

		//return material;
	}

	std::string ModelLoader::ProcessPath(aiString * aPath, std::string directory)
	{
		std::string path = std::string(aPath->C_Str());
		// parse path directly if path contains "/" indicating it is an absolute path;  otherwise 
		// parse as relative.
		if (path.find(":/") == std::string::npos || path.find(":\\") == std::string::npos)
			path = directory + "/" + path;
		return path;
	}

}
