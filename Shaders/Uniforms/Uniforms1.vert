#version 430 core

layout(location = 5) in vec4 pos;
layout(location = 1) uniform vec4 uniformv2;

out vec4 vCol;

void main(void)
{
	gl_Position = pos; 
	vCol = uniformv2;
}