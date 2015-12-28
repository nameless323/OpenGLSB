#version 430 core

layout (location = 0) out vec4 col;

in VS_OUT
{
	vec3 N;
	vec3 L;
	vec3 V;
} fs_in;

uniform vec3 diffuse_albedo = vec3(0.3, 0.5, 0.2);
uniform vec3 specular_albedo = vec3(0.7);
uniform float specular_power = 128.0;
uniform vec3 rim_color = vec3(0.1, 0.2, 0.2);
uniform float rim_power = 5.0;

vec3 CalcRim(vec3 N, vec3 V)
{
	float f = 1.0 - dot(N, V);
	f = smoothstep(0.0, 1.0, f);
	f = pow(f, rim_power);
	
	return f * rim_color; 
}

void main()
{
	vec3 N = normalize(fs_in.N);
	vec3 L = normalize(fs_in.L);
	vec3 V = normalize(fs_in.V);

	vec3 R = reflect(-L, N);

	vec3 diff = max(dot(N, L), 0.0) * diffuse_albedo;
	vec3 spec = pow(max(dot(R, V), 0.0), specular_power) * specular_albedo;
	vec3 rim = CalcRim(N, V);

	col = vec4(diff + spec + rim, 1.0);
}