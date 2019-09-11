#version 450 core
out vec4 FragColor;

in vec3 FragPos;
in vec4 ScreenPos;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

layout (std140, binding = 0) uniform Global
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightRadius;

// uniform sampler2D lightShadowMap;
// uniform mat4 lightShadowViewProjection;

const float kPi = 3.14159265;
const float kShininess = 16.0;



void main()
{
    float linear    = 0.09;
    float quadratic = 0.032;

    vec2 TexCoords = (ScreenPos.xy / ScreenPos.w) * 0.5 + 0.5;

    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.0; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    vec3 lightDir  = normalize(lightPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    const float kEnergyConservation = ( 8.0 + kShininess ) / ( 8.0 * kPi ); 
    float  spec = kEnergyConservation * pow(max(dot(Normal, halfwayDir), 0.0), kShininess);
    vec3 specular = lightColor * spec * Specular;

    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lightColor;

    float distance = length(FragPos - lightPos) * (1.0 / lightRadius);
    float attenuation = clamp(1 - dot(distance, distance), 0, 1);
    attenuation = attenuation * attenuation;

    vec3 ambient = 0.05 * Diffuse;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    lighting += ambient + diffuse + specular;
      
    FragColor = vec4(lighting, 1.0);
    // FragColor = vec4(Diffuse, 1.0);
}
