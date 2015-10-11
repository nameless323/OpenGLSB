#version 430 core

layout (location = 0) in vec4 offset;
layout (location = 1) in vec4 color;

out vec4 col;
layout (location = 5) in vec4 inputPosition;

void main(void)
{	
	gl_Position = inputPosition + offset;
	col = color;
}