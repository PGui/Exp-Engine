#version 450 core
layout (location = 0) in vec3 pos;


layout (std140, binding = 0) uniform Global
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

uniform mat4 model;

void main()
{
	gl_Position =  projection * view * model * vec4(pos, 1.0);
}
