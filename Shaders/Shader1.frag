#version 430 core
out vec4 color;

in VS_OUT
{
	vec4 color;
} fsIn;

void main(void)
{
	color = fsIn.color;
}