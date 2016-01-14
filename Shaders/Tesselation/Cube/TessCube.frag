#version 430 core

out vec4 col;

in vec3 normal;

void main()
{
	col = vec4(abs(normal), 1.0);
}