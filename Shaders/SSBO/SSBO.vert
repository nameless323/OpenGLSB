#version 430 core

struct vertex
{
	vec4 pos;
	vec3 col;
};

layout (binding = 0, std430) buffer my_vertices
{
	vertex vertices[];
};

out VS_OUT
{
	vec3 col;
} vsOut;

uniform mat4 transformMat;

void main(void)
{
	gl_Position = transformMat * vertices[gl_VertexID].position;
	vsOut.col = vertices[gl_VertexID].color;
}