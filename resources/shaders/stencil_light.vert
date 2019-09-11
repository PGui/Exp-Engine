#version 450 core
layout (location = 0) in vec3 aPos;

layout (std140, binding = 0) uniform Global
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

uniform mat4 model;

void main(void) 
{
  gl_Position = projection * view * model * vec4(aPos, 1.0);
}
