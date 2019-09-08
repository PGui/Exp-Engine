#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

layout (std140, binding = 0) uniform Global
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform sampler2D lightShadowMap;
uniform mat4 lightShadowViewProjection;

const float kPi = 3.14159265;
const float kShininess = 16.0;

void main()
{
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

     // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  

    // diffuse
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lightColor;
    // specular
    
    const float kEnergyConservation = ( 8.0 + kShininess ) / ( 8.0 * kPi ); 
    float  spec = kEnergyConservation * pow(max(dot(Normal, halfwayDir), 0.0), kShininess);
    vec3 specular = lightColor * spec * Specular;
    // attenuation
    lighting += diffuse + specular;
      
    FragColor = vec4(lighting, 1.0);
}