#ifndef UNIFORMS_H
#define UNIFORMS_H
layout (std140, binding = 0) uniform Global
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
    mat4 viewProjection;
};
#endif