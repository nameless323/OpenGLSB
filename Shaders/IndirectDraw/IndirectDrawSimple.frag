#version 430 core

in vec3 vCol;
out vec4 col;

void main(void)
{
	col = vec4(vCol, 1.0); 
}