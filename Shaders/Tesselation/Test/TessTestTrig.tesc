#version 430 core

layout (vertices = 3) out;

void main()
{
	if (gl_InvocationID == 0)
	{
		gl_TessLevelInner[0] = 50;

		gl_TessLevelOuter[0] = 25;
		gl_TessLevelOuter[1] = 25;
		gl_TessLevelOuter[2] = 25;
	}
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}