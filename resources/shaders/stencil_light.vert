#version 450 core
layout (location = 0) in vec3 aPos;

#include common/uniforms.glsl

uniform mat4 model;

void main(void) 
{
  gl_Position = projection * view * model * vec4(aPos, 1.0);
}
