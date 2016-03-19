#version 430 core

layout (binding = 0) uniform sampler2D s;
layout (location = 2) uniform float exposure;

out vec4 color;

void main()
{
	vec4 c = texture(s, 2.0 * gl_FragCoord.xy / textureSize(s, 0));
	c.xyz = vec3(1.0) - exp(-c.xyz * exposure);
	color = c;
}