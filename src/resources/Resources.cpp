#include "Resources.h"
#include <set>
#include <stack>


namespace Exp
{

	std::map<unsigned, Shader>      Resources::shaders		= std::map<unsigned, Shader>();
	std::map<unsigned, Texture>     Resources::textures		= std::map<unsigned, Texture>();
	std::map<unsigned, TextureCube> Resources::texturesCube	= std::map<unsigned, TextureCube>();
	std::map<unsigned, SceneNode*>  Resources::meshes			= std::map<unsigned, SceneNode*>();

	std::map<unsigned, Shader>& Resources::DebugGetShaders()
	{
		return Resources::shaders;
	}
	// --------------------------------------------------------------------------------------------
	void Resources::Init()
	{

	}

	void Resources::Clean()
	{
		// Scene instances only reference meshes, so destroy them first.
		Scene::Clear();

		// Template hierarchies own their Mesh objects (allocated by the ModelLoader).
		std::set<Mesh*> ownedMeshes;
		for (auto& [id, root] : meshes)
		{
			std::stack<SceneNode*> nodes;
			nodes.push(root);
			while (!nodes.empty())
			{
				SceneNode* node = nodes.top();
				nodes.pop();
				if (node->mesh)
					ownedMeshes.insert(node->mesh);
				for (SceneNode* child : node->GetChildren())
					nodes.push(child);
			}
			delete root; // recursively deletes children
		}
		meshes.clear();

		for (Mesh* mesh : ownedMeshes)
			delete mesh;
	}

	// --------------------------------------------------------------------------------------------
	Shader* Resources::LoadShader(std::string name, std::string vsPath, std::string fsPath, std::vector<std::string> defines)
	{
		unsigned int id = SID(name);

		// if shader already exists, return that handle
		if (Resources::shaders.find(id) != Resources::shaders.end())
			return &Resources::shaders[id];

		Shader shader = ShaderLoader::LoadShader(name, vsPath, fsPath);
		Resources::shaders[id] = shader;
		return &Resources::shaders[id];
	}

	Shader* Resources::ReloadShader(std::string name)
	{
		unsigned int id = SID(name);

		if (Resources::shaders.find(id) == Resources::shaders.end())
		{
			return nullptr;
			
		}

		Resources::shaders[id].DeleteProgram();
		Resources::shaders[id] = ShaderLoader::LoadShader(name, Resources::shaders[id].vertexFilePath, Resources::shaders[id].fragmentFilePath);
		return &Resources::shaders[id];
			
	}
	// --------------------------------------------------------------------------------------------
	Shader* Resources::GetShader(std::string name)
	{
		unsigned int id = SID(name);

		// if shader exists, return that handle
		if (Resources::shaders.find(id) != Resources::shaders.end())
		{
			return &Resources::shaders[id];
		}
		else
		{
			spdlog::error("Shader {} not found", name);
			return nullptr;
		}
	}
	// --------------------------------------------------------------------------------------------
	Texture* Resources::LoadTexture(std::string name, std::string path, GLenum target, GLenum format, bool srgb)
	{
		unsigned int id = SID(name);

		// if texture already exists, return that handle
		if (Resources::textures.find(id) != Resources::textures.end())
			return &Resources::textures[id];

		spdlog::info("Loading texture file at {}", path);

		Texture texture = TextureLoader::loadTexture(path, target, format, srgb);

		spdlog::info("Successfully loaded {}", path);

		// make sure texture got properly loaded
		if (texture.width > 0)
		{
			Resources::textures[id] = texture;
			return &Resources::textures[id];
		}
		else
		{
			return nullptr;
		}
	}

	// --------------------------------------------------------------------------------------------
	Texture* Resources::GetTexture(std::string name)
	{
		unsigned int id = SID(name);

		// if shader exists, return that handle
		if (Resources::textures.find(id) != Resources::textures.end())
		{
			return &Resources::textures[id];
		}
		else
		{
			spdlog::error("Requested texture {} not found", name);
			return nullptr;
		}
	}

	TextureCube* Resources::LoadTextureCube(std::string name, std::string folder)
	{
		unsigned int id = SID(name);

		// if texture already exists, return that handle
		if (Resources::texturesCube.find(id) != Resources::texturesCube.end())
			return &Resources::texturesCube[id];

		TextureCube texture = TextureLoader::LoadTextureCube(folder);
		Resources::texturesCube[id] = texture;
		return &Resources::texturesCube[id];
	}

	TextureCube* Resources::GetTextureCube(std::string name)
	{
		unsigned int id = SID(name);

		// if shader exists, return that handle
		if (Resources::texturesCube.find(id) != Resources::texturesCube.end())
		{
			return &Resources::texturesCube[id];
		}
		else
		{
			spdlog::error("Requested texture cube {} not found", name);
			return nullptr;
		}
	}


	// --------------------------------------------------------------------------------------------
	SceneNode* Resources::LoadMesh(Renderer* renderer, std::string name, std::string path)
	{
		unsigned int id = SID(name);

		// if Mesh's scene node was already loaded before, copy the scene node's memory and return 
		// the copied reference. We return a copy as the moment the global scene deletes the 
		// returned node, all other and next requested scene nodes of this model will end up as
		// dangling pointers.
		if (Resources::meshes.find(id) != Resources::meshes.end())
		{
			return Scene::MakeSceneNode(Resources::meshes[id]);
		}

		// MeshLoader::LoadMesh initializes a scene node hierarchy on the heap. We are responsible 
		// for managing the memory; keep a reference to the root node of the model scene. 
		SceneNode* node = ModelLoader::LoadMesh(renderer, path);
		Resources::meshes[id] = node;

		// return a copied reference through the scene to prevent dangling pointers. 
		// See description above.
		return Scene::MakeSceneNode(node);
	}
	// --------------------------------------------------------------------------------------------
	SceneNode* Resources::GetMesh(std::string name)
	{
		unsigned int id = SID(name);

		// if Mesh's scene node was already loaded before, copy the scene node's memory and return 
		// the copied reference. We return a copy as the moment the global scene deletes the 
		// returned node, all other and next requested scene nodes of this model will end up as
		// dangling pointers.
		if (Resources::meshes.find(id) != Resources::meshes.end())
		{
			return Scene::MakeSceneNode(Resources::meshes[id]);
		}
		else
		{
			spdlog::error("Requested Mesh {} not found", name);
			return nullptr;
		}
	}
}
