#version 330 core
struct Material
{
	sampler2D diffuse;
    sampler2D specular;
    sampler2D normals;
};


out vec4 FragColor;

uniform Material material;

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;
in vec2 TexCoords;

void main()
{    
    //Return result
    FragColor = texture(material.diffuse, TexCoords);
}
