#pragma once

#include <map>
#include <string>
#include <vector>
#include <spdlog/spdlog.h>
#include <stack>

#include "../hash/Hash.h"

#include "ShaderLoader.h"
#include "TextureLoader.h"
//#include "MeshLoader.h"

#include "../Rendering/Shader.h"
#include "../Rendering/Material.h"
#include "../Rendering/Texture.h"
#include "../Rendering/TextureCube.h"
#include "../Mesh/Mesh.h"

#include "../Scene/Scene.h"
#include "../Scene/SceneNode.h"

#include "ModelLoader.h"

namespace Exp
{
	class SceneNode;
	class Renderer;
	class Shader;
	class Texture;
	class TextureCube;
	class Material;

	/*

	  Global resource manager. Manages and maintains all resource memory used throughout the
	  rendering application. New resources are loaded from here, and duplicate resource loads
	  are prevented. Every resource is referenced by a hashed string ID.

	*/
	class Resources
	{
	private:
		// we index all resources w/ a hashed string ID
		static std::map<unsigned, Shader>		m_Shaders;
		static std::map<unsigned, Texture>		m_Textures;
		static std::map<unsigned, TextureCube>  m_TexturesCube;
		static std::map<unsigned, SceneNode*>	m_Meshes;
	public:

	private:
		// disallow creation of any Resources object; it's defined as a static object
		Resources();
	public:
		// Debug
		static std::map<unsigned, Shader>& DebugGetShaders();
	public:
		static void Init();
		static void Clean();

		static Shader*      LoadShader(std::string name, std::string vsPath, std::string fsPath, std::vector<std::string> defines = std::vector<std::string>());
		static Shader*		ReloadShader(std::string name);
		static Shader*      GetShader(std::string name);

		static Texture*     LoadTexture(std::string name, std::string path, GLenum target = GL_TEXTURE_2D, GLenum format = GL_RGBA, bool srgb = false);
		static Texture*     GetTexture(std::string name);

		static TextureCube* LoadTextureCube(std::string name, std::string folder);
		static TextureCube* GetTextureCube(std::string name);

		static SceneNode*  LoadMesh(Renderer* renderer, std::string name, std::string path);
		static SceneNode*  GetMesh(std::string name);
	};
}
