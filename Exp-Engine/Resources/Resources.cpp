#include "Resources.h"


namespace Exp
{

	std::map<unsigned, Shader>      Resources::m_Shaders		= std::map<unsigned, Shader>();
	std::map<unsigned, Texture>     Resources::m_Textures		= std::map<unsigned, Texture>();
	std::map<unsigned, TextureCube> Resources::m_TexturesCube	= std::map<unsigned, TextureCube>();
	std::map<unsigned, SceneNode*>  Resources::m_Meshes			= std::map<unsigned, SceneNode*>();

	std::map<unsigned, Shader>& Resources::DebugGetShaders()
	{
		return Resources::m_Shaders;
	}
	// --------------------------------------------------------------------------------------------
	void Resources::Init()
	{

	}

	void Resources::Clean()
	{
		// traverse all stored Mesh scene nodes and delete accordingly.
		// Note that this time we don't care about deleting dangling pointers as each scene node is
		// unique and shouldn't reference other scene nodes than their children.
		for (auto it = m_Meshes.begin(); it != m_Meshes.end(); it++)
		{
			delete it->second;
		}
	}

	// --------------------------------------------------------------------------------------------
	Shader* Resources::LoadShader(std::string name, std::string vsPath, std::string fsPath, std::vector<std::string> defines)
	{
		unsigned int id = SID(name);

		// if shader already exists, return that handle
		if (Resources::m_Shaders.find(id) != Resources::m_Shaders.end())
			return &Resources::m_Shaders[id];

		Shader shader = ShaderLoader::LoadShader(name, vsPath, fsPath);
		Resources::m_Shaders[id] = shader;
		return &Resources::m_Shaders[id];
	}

	Shader* Resources::ReloadShader(std::string name)
	{
		unsigned int id = SID(name);

		if (Resources::m_Shaders.find(id) == Resources::m_Shaders.end())
		{
			return nullptr;
			
		}

		Resources::m_Shaders[id].DeleteProgram();
		Resources::m_Shaders[id] = ShaderLoader::LoadShader(name, Resources::m_Shaders[id].vertexFilePath, Resources::m_Shaders[id].fragmentFilePath);
		return &Resources::m_Shaders[id];
			
	}
	// --------------------------------------------------------------------------------------------
	Shader* Resources::GetShader(std::string name)
	{
		unsigned int id = SID(name);

		// if shader exists, return that handle
		if (Resources::m_Shaders.find(id) != Resources::m_Shaders.end())
		{
			return &Resources::m_Shaders[id];
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
		if (Resources::m_Textures.find(id) != Resources::m_Textures.end())
			return &Resources::m_Textures[id];

		spdlog::info("Loading texture file at {}", path);

		Texture texture = TextureLoader::loadTexture(path, target, format, srgb);

		spdlog::info("Successfully loaded {}", path);

		// make sure texture got properly loaded
		if (texture.Width > 0)
		{
			Resources::m_Textures[id] = texture;
			return &Resources::m_Textures[id];
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
		if (Resources::m_Textures.find(id) != Resources::m_Textures.end())
		{
			return &Resources::m_Textures[id];
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
		if (Resources::m_TexturesCube.find(id) != Resources::m_TexturesCube.end())
			return &Resources::m_TexturesCube[id];

		TextureCube texture = TextureLoader::LoadTextureCube(folder);
		Resources::m_TexturesCube[id] = texture;
		return &Resources::m_TexturesCube[id];
	}

	TextureCube* Resources::GetTextureCube(std::string name)
	{
		unsigned int id = SID(name);

		// if shader exists, return that handle
		if (Resources::m_TexturesCube.find(id) != Resources::m_TexturesCube.end())
		{
			return &Resources::m_TexturesCube[id];
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
		if (Resources::m_Meshes.find(id) != Resources::m_Meshes.end())
		{
			return Scene::MakeSceneNode(Resources::m_Meshes[id]);
		}

		// MeshLoader::LoadMesh initializes a scene node hierarchy on the heap. We are responsible 
		// for managing the memory; keep a reference to the root node of the model scene. 
		SceneNode* node = ModelLoader::LoadMesh(renderer, path);
		Resources::m_Meshes[id] = node;

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
		if (Resources::m_Meshes.find(id) != Resources::m_Meshes.end())
		{
			return Scene::MakeSceneNode(Resources::m_Meshes[id]);
		}
		else
		{
			spdlog::error("Requested Mesh {} not found", name);
			return nullptr;
		}
	}
}
