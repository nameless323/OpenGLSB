#version 430 core
out vec4 color;

layout (location = 3) uniform float LerpVal;
layout (location = 20) uniform vec4 newCol[2];
layout (location = 5) uniform float lerpValSin;

in vec4 col;

void main(void)
{
	vec4 lerpc = mix(newCol[0], newCol[1], LerpVal);
	color = mix(col, lerpc, lerpValSin);
}