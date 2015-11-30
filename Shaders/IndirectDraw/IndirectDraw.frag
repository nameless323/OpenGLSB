#version 430 core

in VS_OUT
{
	vec3 normal;
	vec4 color;
} fs_in;

out vec4 col;

void main(void)
{

	vec3 n = normalize(fs_in.normal);
	col = fs_in.color * abs(n.z);
}