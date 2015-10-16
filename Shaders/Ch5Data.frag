#version 430 core
out vec4 color;

layout (location = 2) uniform vec4 NewCol;

in vec4 col;

void main(void)
{
	color = NewCol;
}