#version 430 core
layout(location = 1) uniform sampler2D s;
out vec4 col;

void main(void)
{
	col = texture2D(s, gl_FragCoord.xy / textureSize(s, 0));
	//col = texture(s, gl_FragCoord.xy / textureSize(s, 0)); 
}