#version 430 core

layout(location = 0) in vec4 pos;
layout(location = 4) in vec2 vUv;

out vec2 uv;

void main(void)
{
	gl_Position = pos;
	uv = vUv;
}