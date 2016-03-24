#version 430 core

layout (location = 0) out vec4 color0;
layout (location = 1) out vec4 color1;

in VS_OUT
{
	vec3 N;
	vec3 L;
	vec3 V;
	flat int material_index;
} fs_in;

layout (location = 4) uniform float bloom_thresh_min = 0.8;
layout (location = 5) uniform float bloom_thresh_max = 1.2;

struct material_t
{
	vec3 diffuse_color;
	vec3 specular_color;
	float specular_power;
	vec3 ambient_color;
};

layout (binding = 1, std140) uniform MATERIAL_BLOCK
{
	material_t material[32];
} materials;

void main()
{
	vec3 N = normalize(fs_in.N);
	vec3 L = normalize(fs_in.L);
	vec3 V = normalize(fs_in.V);

	vec3 R = reflect(-L, N);

	material_t m = materials.material[fs_in.material_index];

	vec3 diffuse = max(dot(N, L), 0.0) * m.diffuse_color;
	vec3 specular = pow(max(dot(R, V), 0.0), m.specular_power) * m.specular_color;
	vec3 ambient = m.ambient_color;

	vec3 color = ambient + diffuse + specular;

	color0 = vec4(color, 1.0);

	float Y = dot(color, vec3(0.299, 0.587, 0.144));

	color = color * 4.0 * smoothstep(bloom_thresh_min, bloom_thresh_max, Y);
	color1 = vec4(color, 1.0);
}