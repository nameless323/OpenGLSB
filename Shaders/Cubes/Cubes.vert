#version 430 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 col;

out vec4 vCol;

void main(void)
{
	gl_Position = vec4(pos, 1.0);
	vCol = col;
}