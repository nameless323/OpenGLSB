#version 430 core

layout (triangles) in;
layout (points, max_vertices = 3) out;

in vec4 teColor[];
out vec4 geomColor;

void main(void)
{
	int i;
	for (i = 0; i < gl_in.length(); i++)
	{
		gl_Position = gl_in[i].gl_Position;
		geomColor = teColor[i];
		EmitVertex();
	}
}