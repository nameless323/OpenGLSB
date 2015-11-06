#version 430 core

layout (location = 0) out vec4 color;

in VS_OUT
{
	flat int alien;
	vec2 tc;
} fsIn;

layout (binding = 0) uniform sampler2DArray texAliens;

void main(void)
{
	color = texture(texAliens, vec3(fsIn.tc, float(fsIn.alien)));
}