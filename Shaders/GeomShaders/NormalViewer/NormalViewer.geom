#version 430 core

layout (triangles) in;
layout (line_strip, max_vertices = 8) out;

uniform mat4 mvp;
uniform float normal_length;

in Vertex
{
	vec3 normal;
} vertex[];

void main()
{
	for (int i = 0; i < 3; i++)
	{
		gl_Position = mvp * gl_in[i].gl_Position;
		EmitVertex();
		gl_Position = mvp * vec4(gl_in[i].gl_Position.xyz + vertex[i].normal*normal_length, 1.0);
		EmitVertex();
		EmitPrimitive();
	}
	vec4 newPos = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0;
	gl_Position = mvp * newPos;
	EmitVertex();
	vec3 a = normalize(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz);
	vec3 b = normalize(gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz);
	vec3 faceNormal = cross(a, b);
	gl_Position = mvp * (newPos + vec4(faceNormal * normal_length, 1.0));
	EmitVertex();
	EmitPrimitive();
}