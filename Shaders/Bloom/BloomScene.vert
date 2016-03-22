#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

out VS_OUT
{
	vec3 N;
	vec3 L;
	vec3 V;
	flat int material_index;
} vs_out;

uniform vec3 light_pos = vec3(100.0, 100.0, 100.0);

layout (binding = 0, std140) uniform TRANSFORM_BLOCK
{
	mat4 mat_proj;
	mat4 mat_view;
	mat4 mat_model[32];
} transforms;

void main()
{
	mat4 mat_mv = transforms.mat_view * transforms.mat_model[gl_InstanceID];
	vec4 P = mat_mv * position;

	vs_out.N = mat3(mat_mv) * normal;
	vs_out.L = light_pos - P.xyz;
	vs_out.V = -P.xyz;

	vs_out.material_index = gl_InstanceID;
	gl_Position = transforms.mat_proj * P;
}