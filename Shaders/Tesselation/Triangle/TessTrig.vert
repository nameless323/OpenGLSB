#version 430 core

void main()
{
	const vec4 verts[] = vec4[]
						(
							vec4(0.25, -0.25, 0.5, 1.0),
							vec4(-0.25, -0.25, 0.5, 1.0),
							vec4(0.25, 0.25, 0.5, 1.0)
						);
	gl_Position = verts[gl_VertexID];
}