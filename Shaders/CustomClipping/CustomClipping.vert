#version 430 core

layout (location = 0) in vec4 pos;
layout (location = 1) in vec3 norm;

uniform mat4 mv;
uniform mat4 proj;

out VS_OUT
{
	vec3 N;
	vec3 L;
	vec3 V;
} vs_out;

uniform vec3 lightPos = vec3(100.0, 100.0, 100.0);

uniform vec4 clipPlane = vec4(1.0, 1.0, 0.0, 0.85);
uniform vec4 clipSphere = vec4(0.0, 0.0, 0.0, 0.85);

void main(void)
{
	vec4 P = mv * pos;
	vs_out.N = mat3(mv) * norm;
	vs_out.L = lightPos - P.xyz;
	vs_out.V = -P.xyz;

	gl_ClipDistance[0] = dot(pos, clipPlane);
	gl_ClipDistance[1] = length(pos.xyz/pos.w - clipSphere.xyz) - clipSphere.w;

	gl_Position = proj * P;

}