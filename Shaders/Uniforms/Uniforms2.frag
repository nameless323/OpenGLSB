#version 430 core

layout(location = 0) uniform vec4 uniformColor;

layout(shared, binding = 0) uniform UniBlockStd
{
	float col;
	vec3 colVec;
	float[3] colArr;
	mat4 colMat;
} stdVal;

layout(std140, binding = 1) uniform ShadedBlockStd
{
	float col;
	vec4 colVec;
	float[3] colArr;
	mat3 colMat;
} sharVal;

out vec4 color;
void main(void)
{
	//color = vec4(sharVal.colArr[0], sharVal.colArr[1], sharVal.colArr[2], 1.0);
	color = vec4(sharVal.colMat[1], 1.0);
}