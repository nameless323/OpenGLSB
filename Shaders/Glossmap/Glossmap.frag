#version 420 core

layout (binding = 0) uniform sampler3D tex_envmap;
layout (binding = 1) uniform sampler2D tex_glossmap;

in VS_OUT
{
	vec3 normal;
	vec3 view;
	vec2 tc;
} fs_in;

out vec4 color;

void main()
{
	vec3 u = normalize(fs_in.view);
	vec3 r = reflect(u, normalize(fs_in.normal));
	r.z += 1.0;
	float m = 0.5 * inversesqrt(dot(r, r));

	float gloss = texture(tex_glossmap, fs_in.tc * vec2(3.0, 1.0) * 2.0).r;

	vec3 env_coord = vec3(r.xy * m + vec2(0.5), gloss);
	color = texture(tex_envmap, env_coord);
}