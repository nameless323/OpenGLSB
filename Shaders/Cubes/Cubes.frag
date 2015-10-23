#version 430 core

in vec4 vCol;
out vec4 fCol;

void main(void)
{
	fCol = vCol;
	fCol = vec4(0.7, 0.7, 0.7, 0.7);
}
