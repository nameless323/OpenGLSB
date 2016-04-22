#version 430 core

layout (binding = 0) uniform sampler2D sColor;
layout (binding = 1) uniform sampler2D sNormalDepth;

layout (location = 0) out vec4 color;

uniform float ssao_level = 1.0;
uniform float object_level = 1.0;
uniform float ssao_radius = 5.0;
uniform bool weight_by_angle = true;
uniform uint point_count = 8;
uniform bool randomize_points = true;

layout (binding = 0, std140) uniform SAMPLE_POINTS
{
	vec4 pos[256];
	vec4 random_vectors[256];
} points;

void main()
{
	vec2 P = gl_FragCoord.xy / textureSize(sNormalDepth, 0);
	vec4 ND = textureLod(sNormalDepth, P, 0);

	vec3 N = ND.xyz;
	float my_depth = ND.w;

	int i;
	int j;
	int n;

	float occ = 0.0;
	float total = 0.0;

	n = (int(gl_FragCoord.x * 7123.2315 + 125.232) *
		int (gl_FragCoord.y * 3137.1529 + 234.8)) ^
		int(my_depth);

	vec4 v = points.random_vectors[n & 255];

	float r = (v.r + 3.0) * 0.1;
	if (!randomize_points)
		r = 0.5;
	for (i = 0; i < point_count; i++)
	{
		vec3 dir = points.pos[i].xyz;
		if (dot(N, dir) < 0.0)
			dir = -dir;

		float f = 0.0;
		float z = my_depth;

		total += 4;

		for (j = 0; j < 4; j++)
		{
			f += r;
			z -= dir.z *f;

			float their_depth =
				textureLod(sNormalDepth, P + dir.xy * f * ssao_radius, 0).w;
			float d = abs (their_depth - my_depth);
			d *= d;

			if ((z - their_depth) > 0.0)
				occ += 4.0 / (1.0 + d);
		}
	}
	float ao_amount = (1.0 - occ / total);
	vec4 object_color = textureLod(sColor, P, 0);
	color = object_level * object_color + mix(vec4(0.2), vec4(ao_amount), ssao_level);
}