#version 430 core

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 normal_depth;

in VS_OUT
{
	vec3 N;
	vec3 L;
	vec3 V;
} fs_in;

uniform vec3 diffuse_albedo = vec3(0.8, 0.8, 0.9);
uniform vec3 specular_albedo = vec3(0.01);
uniform float specular_power = 128.0;
uniform float shading_level = 1.0;

void main()
{
	vec3 N = normalize(fs_in.N);
	vec3 L = normalize(fs_in.L);
	vec3 V = normalize(fs_in.V);

	vec3 R = reflect(-L, N);

	vec3 diffuse = max(dot(N, L), 0.0) * diffuse_albedo;
	diffuse *= diffuse;
	vec3 specular = pow(max(dot(R, V), 0.0), specular_power) * specular_albedo;
	color = mix(vec4(0.0), vec4(diffuse + specular, 1.0), shading_level);
	normal_depth = vec4(N, fs_in.V.z);
}