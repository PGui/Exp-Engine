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

// Exposure (linear multiplier applied before tonemapping)
uniform float Exposure;

// 0 = none (clamp), 1 = Reinhard (luminance), 2 = ACES (Narkowicz fit), 3 = ACES (Hill fit, RRT+ODT)
uniform int Tonemapper;

// Output encoding: exact sRGB transfer function, or pow(1/Gamma) when SRGBOutput is off
uniform bool SRGBOutput;
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

// Reinhard on luminance: compresses highlights without desaturating colours.
vec3 reinhardLuminance(vec3 col)
{
    float l = dot(col, vec3(0.2126, 0.7152, 0.0722));
    return col / (1.0 + l);
}

// https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl (Stephen Hill)
vec3 acesHill(vec3 col)
{
    const mat3 inputMat = mat3(
        0.59719, 0.07600, 0.02840,
        0.35458, 0.90834, 0.13383,
        0.04823, 0.01566, 0.83777);
    const mat3 outputMat = mat3(
         1.60475, -0.10208, -0.00327,
        -0.53108,  1.10813, -0.07276,
        -0.07367, -0.00605,  1.07602);
    col = inputMat * col;
    vec3 a = col * (col + 0.0245786) - 0.000090537;
    vec3 b = col * (0.983729 * col + 0.4329510) + 0.238081;
    col = outputMat * (a / b);
    return clamp(col, 0.0, 1.0);
}

vec3 linearToSRGB(vec3 col)
{
    vec3 lo = col * 12.92;
    vec3 hi = 1.055 * pow(col, vec3(1.0 / 2.4)) - 0.055;
    return mix(hi, lo, vec3(lessThanEqual(col, vec3(0.0031308))));
}

void main()
{
    vec3 color = texture(TexSrc, TexCoords).rgb;
    vec3 grayscale = vec3(dot(color, vec3(0.299, 0.587, 0.114)));
    vec2 texelSize = 1.0 / textureSize(TexSrc, 0).xy;    
    
    // HDR -> LDR: a single tonemapping operator (chaining two crushes the whites).
    color *= Exposure;
    if (Tonemapper == 1)
        color = reinhardLuminance(color);
    else if (Tonemapper == 2)
        color = aces(color);
    else if (Tonemapper == 3)
        color = acesHill(color);
    color = clamp(color, 0.0, 1.0);

    // Linear -> display encoding (exactly once: textures are decoded by the GPU as sRGB).
    color = SRGBOutput ? linearToSRGB(color) : pow(color, vec3(1.0 / Gamma));
    
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