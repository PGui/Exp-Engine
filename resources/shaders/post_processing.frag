#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D TexSrc;

// post-process effect toggles
uniform int Vignette;
uniform int Sepia;
uniform int Bloom;
uniform int SSAO;
uniform int MotionBlur;

// Exposure
uniform float Exposure;

// Gamma
uniform float Gamma;

// sepia
const vec3 sepiaColor = vec3(1.2, 1.0, 0.8);

// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 aces(vec3 col)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((col*(a*col+b))/(col*(c*col+d)+e), 0.0, 1.0);
}

void main()
{
    vec3 color = texture(TexSrc, TexCoords).rgb;
    vec3 grayscale = vec3(dot(color, vec3(0.299, 0.587, 0.114)));
    vec2 texelSize = 1.0 / textureSize(TexSrc, 0).xy;    
    
    // HDR tonemapping
    color *= Exposure;
    color = aces(color);
    color = color / (color + vec3(1.0));
	// Gamma correct
	color = pow(color, vec3(1.0/Gamma));     
    
    if(Sepia == 1)
    {
        color = mix(color, grayscale * sepiaColor, 0.7);
    }

    if(Vignette == 1)
    {
        const float strength = 10.0;
        const float power = 0.1;
        vec2 tuv = TexCoords * (vec2(1.0) - TexCoords.yx);
        float vign = tuv.x*tuv.y * strength;
        vign = pow(vign, power);
        color *= vign;
    }
    

    /////// Kernel post process
    // const float offset = 1.0 / 300.0;
    // vec2 offsets[9] = vec2[](
    //     vec2(-offset,  offset), // top-left
    //     vec2( 0.0f,    offset), // top-center
    //     vec2( offset,  offset), // top-right
    //     vec2(-offset,  0.0f),   // center-left
    //     vec2( 0.0f,    0.0f),   // center-center
    //     vec2( offset,  0.0f),   // center-right
    //     vec2(-offset, -offset), // bottom-left
    //     vec2( 0.0f,   -offset), // bottom-center
    //     vec2( offset, -offset)  // bottom-right    
    // );

    // float kernel[9] = float[](
    //     -1, -1, -1,
    //     -1,  9, -1,
    //     -1, -1, -1
    // );
    
    // vec3 sampleTex[9];
    // for(int i = 0; i < 9; i++)
    // {
    //     sampleTex[i] = vec3(texture(TexSrc, TexCoords.st + offsets[i]));
    // }

    // for(int i = 0; i < 9; i++)
    //     color += sampleTex[i] * kernel[i];
    /////// Kernel post process

    FragColor = vec4(color, 1.0);
}