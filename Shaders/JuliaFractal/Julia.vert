#version 430 core

uniform float zoom;
uniform vec2 offset;

out vec2 initial_z;

void main()
{
	const vec4 vertices[4] = vec4[4](vec4(-1.0, -1.0, 0.5, 1.0),
                          vec4( 1.0, -1.0, 0.5, 1.0),
                          vec4( 1.0,  1.0, 0.5, 1.0),
                          vec4(-1.0,  1.0, 0.5, 1.0));
	initial_z = (vertices[gl_VertexID].xy * zoom) + offset;
	gl_Position = vertices[gl_VertexID];
}