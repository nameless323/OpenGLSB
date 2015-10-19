#version 430 core

layout(location = 0) uniform vec4 uniformColor;

layout(std140, binding = 0) uniform UniBlockStd
{
	float col;
	vec4 colVec;
	float[3] colArr;
	mat4 colMat;
} stdVal;

layout(shared, binding = 1) uniform ShadedBlockStd
{
	float col;
	vec4 colVec;
	float[3] colArr;
	mat4 colMat;
} sharVal;

out vec4 color;
void main(void)
{
	color = uniformColor;
}