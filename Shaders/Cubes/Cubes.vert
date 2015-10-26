#version 430 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 col;

out vec4 vCol;
layout(location = 2) uniform mat4 mvMat;
layout(location = 3) uniform mat4 projMat;

void main(void)
{
	gl_Position = projMat * mvMat * vec4(pos, 1.0);
	vCol = vec4(pos, 1.0) * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);
}