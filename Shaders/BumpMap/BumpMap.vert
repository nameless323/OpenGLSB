#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;

layout (location = 4) in vec2 texcoord;

out VS_OUT
{
	vec2 texcoord;
	vec3 eyeDir;
	vec3 lightDir;
	vec3 normal;
} vs_out;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform vec3 light_pos = vec3 (0.0, 0.0, 100.0);

void main()
{
	vec4 P = mv_matrix * position;

	vec3 V = P.xyz;
	vec3 N = normalize(mat3(mv_matrix) * normal);
	vec3 T = normalize(mat3(mv_matrix) * tangent);

	vec3 B = cross(N, T);

	vec3 L = light_pos - P.xyz;
	vs_out.lightDir = normalize(vec3(dot(L, T), dot(L, B), dot(L, N)));
	V = -P.xyz;
	vs_out.eyeDir = normalize(vec3(dot(V, T), dot(V, B), dot(V, N)));

	vs_out.texcoord = texcoord;
	vs_out.normal = N;
	gl_Position = proj_matrix * P;
}