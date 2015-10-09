#version 430 core
out vec4 color;

in vec4 geomColor;

void main(void)
{
	color = geomColor;
}