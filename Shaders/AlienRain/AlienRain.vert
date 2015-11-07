#version 430 core

layout (location = 0) in int alienIndex;

out VS_OUT
{
	flat int alien;
	vec2 tc;
} vsOut;

struct droplet_t
{
	float x_offset;
	float y_offset;
	float orientation;
	float unused;
};

layout (std140) uniform droplets
{
	droplet_t droplet[256];
};

void main(void)
{
	const vec2[4] position = vec2[4]
	(
		vec2(-0.5, -0.5),
		vec2(0.5, -0.5),
		vec2(-0.5, 0.5),
		vec2(0.5, 0.5)
	);
	vsOut.tc = position[gl_VertexID].xy + vec2(0.5);
	vsOut.alien = alienIndex % 64;
	float co = cos(droplet[alienIndex].orientation);
	float so = sin(droplet[alienIndex].orientation);
	mat2 rot = mat2(vec2(co, so), vec2(-so, co));
	vec2 pos = 0.25 * rot * position[gl_VertexID];
	gl_Position = vec4(pos.x + droplet[alienIndex].x_offset, pos.y + droplet[alienIndex].y_offset, 0.5, 1.0);
}