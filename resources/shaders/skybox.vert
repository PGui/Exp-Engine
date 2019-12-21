#version 450 core
layout (location = 0) in vec3 aPos;

#include common/uniforms.glsl

out vec3 WorldPos;

void main()
{
    WorldPos = aPos;
	mat4 rotView = mat4(mat3(view));
	vec4 clipPos = projection * rotView * vec4(WorldPos, 1.0);
	gl_Position = clipPos.xyww;
}