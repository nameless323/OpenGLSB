#version 430 core
layout(location = 1) uniform sampler2D s;
out vec4 col;

void main(void)
{
	//col = texture2D(s, ivec2(gl_FragCoord.xy));
	//col = texture(s, gl_FragCoord.xy / textureSize(s, 0)); 
	col = texelFetch(s, ivec2(gl_FragCoord.xy / vec2(800, 600) *textureSize(s, 0) ), 0);
}