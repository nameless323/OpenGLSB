#version 430 core

uniform sampler1D tex_toon;
uniform vec3 light_pos = vec3(30.0, 30.0, 100.0);

in VS_OUT
{
	vec3 normal;
	vec3 view;
} fs_in;

out vec4 color;

void main()
{
	vec3 N = normalize(fs_in.normal);
	vec3 L = normalize(light_pos - fs_in.view);

	float tc = pow(max(0.0, dot(N, L)), 5.0);

	color = texture(tex_toon, tc) * (tc * 0.8 + 0.2);
}