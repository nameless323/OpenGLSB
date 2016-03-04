#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out VS_OUT
{
	vec3 normal;
	vec4 color;
} vs_out;

void main()
{
	gl_Position = vec4(position, 1.0);
	vs_out.normal = normal;
	vs_out.color = vec4(position, 1.0) * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);
}