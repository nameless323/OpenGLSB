#version 430 core

layout (location = 0) in vec3 pos;
in mat4 model;
layout (location = 2) in vec3 col;

out vec3 vCol;

void main(void)
{
	vCol = col;
	gl_Position = model * vec4(pos, 1.0);
}