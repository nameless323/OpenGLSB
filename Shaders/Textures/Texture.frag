#version 430 core
layout(location = 1) uniform sampler2D s;
layout(location = 2) uniform sampler2D s2;
layout(location = 3) uniform float t;
out vec4 col;

void main(void)
{
	//col = texture2D(s, ivec2(gl_FragCoord.xy));
	//col = texture(s, gl_FragCoord.xy / textureSize(s, 0)); 
	vec4 cols = texelFetch(s, ivec2(gl_FragCoord.xy / vec2(800, 600) *textureSize(s, 0) ), 0);
	vec4 cols2 = texelFetch(s2, ivec2(gl_FragCoord.xy / vec2(800, 600) *textureSize(s2, 0) ), 0);
	col = mix(cols, cols2, t);
}