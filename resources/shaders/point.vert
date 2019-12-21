#version 450 core
layout (location = 0) in vec3 aPos;

out vec4 ScreenPos;
out vec3 FragPos;

#include common/uniforms.glsl

uniform mat4 model;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    ScreenPos = projection * view * vec4(FragPos, 1.0);
    
	gl_Position = ScreenPos;
}
