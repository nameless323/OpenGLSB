#version 430 core

uniform samplerCube tex_cubemap;

in VS_OUT
{
	vec3 tc;
} fs_in;

layout (location = 0) out vec4 color;

void main()
{
	color = texture(tex_cubemap, fs_in.tc);
}