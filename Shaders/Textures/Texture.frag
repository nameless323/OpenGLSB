#version 430 core
layout(location = 1) uniform sampler2D s;
layout(location = 2) uniform sampler2D s2;
layout(location = 3) uniform float t;

in vec2 uv;

out vec4 col;

void main(void)
{
	//col = texture2D(s, ivec2(gl_FragCoord.xy));
	//col = texture(s, gl_FragCoord.xy / textureSize(s, 0)); 
	/*vec4 cols = texture(s, uv);
	vec4 cols2 = texture(s2, uv);
	col = mix(cols, cols2, t);*/
	col = texture(s, uv);
}