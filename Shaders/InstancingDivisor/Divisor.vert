#version 430 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 instancedPosition;
layout(location = 2) in vec4 instancedColor;

layout(location = 3) in mat4 mvp;

out vec4 vCol;

void main(void)
{
	gl_Position = mvp * (pos + instancedPosition);
	vCol = instancedColor;
}