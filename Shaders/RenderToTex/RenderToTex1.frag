#version 430 core

in VS_OUT
{
	vec4 color;
	vec2 texcoord;
} fs_in;

out vec4 color;

void main()
{
	color = sin (fs_in.color * vec4(40.0, 20.0, 30.0, 1.0)) * 0.5 + vec4(0.5);
	color = fs_in.color;
}