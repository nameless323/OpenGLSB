#version 430 core

layout(location = 5) in vec4 pos;

void main(void)
{
	gl_Position = pos; 
}