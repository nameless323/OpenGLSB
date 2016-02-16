#version 430 core

layout (vertices = 3) out;

void main()
{
	if (gl_InvocationID == 0)
	{
		gl_TessLevelInner[0] = 6;

		gl_TessLevelOuter[0] = 6;
		gl_TessLevelOuter[1] = 6;
		gl_TessLevelOuter[2] = 6;
	}
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}