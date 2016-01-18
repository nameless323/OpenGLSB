#version 430 core

layout (isolines) in;

void main()
{
	float r = (gl_TessCoord.y + gl_TessCoord.x / gl_TessLevelOuter[0]);
	float t = gl_TessCoord.x * 2.0 * 3.14159;
	gl_Position = vec4(sin(t) * r, cos(t) * r, 0.5, 1.0);
}