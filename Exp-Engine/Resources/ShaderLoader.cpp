#include "ShaderLoader.h"
#include "../Rendering/Shader.h"

namespace Exp
{
	Shader ShaderLoader::LoadShader(std::string name, std::string vsFile, std::string fsFile)
	{
        std::ifstream vShaderFile, fShaderFile;

        vShaderFile.exceptions(std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::badbit);

        try
        {
            vShaderFile.open(vsFile);
            fShaderFile.open(fsFile);

            // if either of the two files don't exist, return w/ error message
            if (!vShaderFile.is_open() || !fShaderFile.is_open())
            {
                spdlog::error("Shader failed to load at path: " + vsFile + " and " + fsFile);
                return Shader();
            }

            // retrieve directory (for relative paths in shader includes)
            std::string vsDirectory = vsFile.substr(0, vsFile.find_last_of("/\\"));
            std::string fsDirectory = fsFile.substr(0, fsFile.find_last_of("/\\"));
            std::string vsSource = ReadShader(vShaderFile, name, vsFile);
            std::string fsSource = ReadShader(fShaderFile, name, fsFile);

            vShaderFile.close();
            fShaderFile.close();

            return Shader(name, vsSource, fsSource, vsFile, fsFile);;
        }
        catch (std::ifstream::failure e)
        {
            spdlog::error("SHADER::FILE_NOT_SUCCESFULLY_READ. {}/{}. Error {}", vsFile, fsFile, e.what());
        }
        
        return Shader();
	}
    std::string ShaderLoader::ReadShader(std::ifstream& file, const std::string& name, std::string path)
    {
        std::string directory = path.substr(0, path.find_last_of("/\\"));
        std::string source, line;
        while (std::getline(file, line))
        {
            // if we encounter an #include line, include other shader source
            if (line.substr(0, 8) == "#include")
            {
                std::string includePath = directory + "/" + line.substr(9);
                std::ifstream includeFile(includePath);
                if (includeFile.is_open())
                {
                    // we recursively read the shader file to support any shader include depth
                    source += ReadShader(includeFile, name, includePath);
                }
                else
                {
                    spdlog::error("Shader: " + name + ": include: " + includePath + " failed to open.");
                }
                includeFile.close();
            }
            else
            {
                source += line + "\n";
            }
            
        }
        return source;
    }
}