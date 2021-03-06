#version 430 core

uniform sampler2D tex_envmap;

in VS_OUT
{
	vec3 normal;
	vec3 view;
} fs_in;

out vec4 color;

void main()
{
	vec3 u = normalize(fs_in.view);
	vec3 r = reflect(u, normalize(fs_in.normal));

	vec2 tc;

	tc.y = r.y;
	r.y = 0.0;
	tc.x = normalize(r).x * 0.5;

	float s = sign(r.z) * 0.5;
	tc.s = 0.75 - s * (0.5 - tc.s);
	tc.t = 0.5 + 0.5 * tc.t;

	color = texture(tex_envmap, tc);
}