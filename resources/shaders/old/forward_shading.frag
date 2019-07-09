#version 330 core
struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

//Directional lights
uniform int dirLightCount;
struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NR_DIR_LIGHTS 4
uniform DirLight dirLights[NR_DIR_LIGHTS];

//Point lights
uniform int pointLightCount;
struct PointLight
{
	vec3 position;

	vec3 lightColor;

    float constant;
    float linear;
    float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NR_POINT_LIGHTS 8
uniform PointLight pointLights[NR_POINT_LIGHTS];

//Spot lights
uniform int spotLightCount;
struct SpotLight
{
	vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NR_SPOT_LIGHTS 8
uniform SpotLight spotLights[NR_SPOT_LIGHTS];


out vec4 FragColor;

uniform Material material;

uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;
in vec2 TexCoords;

float specularStrength = 0.5;

vec3 ComputeDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 ComputePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 ComputeSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    vec3 result = texture(material.diffuse, TexCoords).rgb * 0.1;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos-FragPos);

    //Dirlight
	for (int i = 0; i < dirLightCount; ++i)
	{
		result += ComputeDirLight(dirLights[i], norm, viewDir);
	}
    
    //Point lights
    for (int i = 0; i < pointLightCount; ++i)
    {
        result += ComputePointLight(pointLights[i], norm, FragPos, viewDir);
    }
  	
    //Spot lights
	 for (int i = 0; i < spotLightCount; ++i)
	 {
		result += ComputeSpotLight(spotLights[i], norm, FragPos, viewDir);
	 }
    
    //Return result
    FragColor = vec4(result, 1.0);
}

vec3 ComputeDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    //diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    //specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot( viewDir, reflectDir ),0.0), material.shininess);

    vec3 ambient = light.ambient  * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    return (ambient + diffuse + specular);
}

vec3 ComputePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    //diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    //spec
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    //attenuation
    float distance = length(fragPos-light.position);
    float attenuation = 1.0 / (light.constant + (light.linear * distance) + (light.quadratic * (distance * distance)));

    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    ambient *= attenuation * light.lightColor;
    diffuse *= attenuation * light.lightColor;
    specular *= attenuation * light.lightColor;
    return (ambient + diffuse + specular);
}

vec3 ComputeSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    //diff
    float diff = max(dot(normal, lightDir), 0.0);

    //spec
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    //distance attenuation
    float distance = length(fragPos-light.position);
    float attenuation = 1.0 / (light.constant + (light.linear * distance) + (light.quadratic * (distance * distance)));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}
