#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

#include common/uniforms.glsl

uniform vec3 lightDir;   // normalized, from the surface towards the light
uniform vec3 lightColor;

// Shadows
uniform bool hasShadow;
uniform sampler2DShadow lightShadowMap;
uniform mat4 lightShadowViewProjection;
uniform float shadowNormalOffset; // world units, pushes the lookup along the normal
uniform float shadowBias;         // depth bias, scaled by the surface slope

const float kPi = 3.14159265;
const float kShininess = 16.0;

float ComputeShadow(vec3 worldPos, vec3 normal, float NdotL)
{
    // Normal offset removes most acne without peter-panning.
    vec3 offsetPos = worldPos + normal * shadowNormalOffset * (1.0 - NdotL);
    vec4 lightClip = lightShadowViewProjection * vec4(offsetPos, 1.0);
    vec3 proj = lightClip.xyz / lightClip.w * 0.5 + 0.5;

    // Beyond the far plane of the light frustum: treat as lit.
    if (proj.z > 1.0)
        return 1.0;

    float bias = shadowBias * (1.0 + 2.0 * (1.0 - NdotL));
    float reference = proj.z - bias;

    // 3x3 PCF on top of the hardware 2x2 comparison filter.
    vec2 texel = 1.0 / vec2(textureSize(lightShadowMap, 0));
    float lit = 0.0;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            lit += texture(lightShadowMap, vec3(proj.xy + vec2(x, y) * texel, reference));
        }
    }
    return lit / 9.0;
}

void main()
{
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float NdotL = max(dot(Normal, lightDir), 0.0);

    // diffuse
    vec3 diffuse = NdotL * Diffuse * lightColor;

    // specular (energy conserving Blinn-Phong)
    const float kEnergyConservation = ( 8.0 + kShininess ) / ( 8.0 * kPi );
    float  spec = kEnergyConservation * pow(max(dot(Normal, halfwayDir), 0.0), kShininess);
    vec3 specular = lightColor * spec * Specular;

    float shadow = (hasShadow && NdotL > 0.0) ? ComputeShadow(FragPos, Normal, NdotL) : 1.0;

    lighting += (diffuse + specular) * shadow;

    FragColor = vec4(lighting, 1.0);
}
