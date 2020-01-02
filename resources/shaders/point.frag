#version 450 core
out vec4 FragColor;

// in vec3 worldPos;
in vec4 ScreenPos;

uniform sampler2D gPositionMetallic;
uniform sampler2D gNormalRoughness;
uniform sampler2D gAlbedoAO;

#include common/uniforms.glsl
#include common/constants.glsl
#include common/brdf.glsl

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightRadius;

// uniform sampler2D lightShadowMap;
// uniform mat4 lightShadowViewProjection;

void main()
{
    vec2 uv = (ScreenPos.xy / ScreenPos.w) * 0.5 + 0.5;
    
    vec4 albedoAO = texture(gAlbedoAO, uv);
    vec4 normalRoughness = texture(gNormalRoughness, uv);
    vec4 positionMetallic = texture(gPositionMetallic, uv);
    
    vec3 worldPos   = positionMetallic.xyz;
    vec3 albedo     = albedoAO.rgb;
    vec3 normal     = normalRoughness.rgb;
    float roughness = normalRoughness.a;
    float metallic  = positionMetallic.a;
    float ao = albedoAO.a;

    vec3 N = normalize(normal);
    vec3 V = normalize(viewPos - worldPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	           
    // reflectance equation
    vec3 Lo = vec3(0.0);
    // calculate per-light radiance
    vec3 L = normalize(lightPos - worldPos);
    vec3 H = normalize(V + L);
    float dst    = length(lightPos - worldPos);
    float attenuation = 1.0 / (dst * dst);
    vec3 radiance     = lightColor * attenuation;        
    
    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, roughness);        
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);       
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular     = numerator / max(denominator, 0.001);  
        
    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);                
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo; 
   
    FragColor = vec4(color, 1.0);
}
