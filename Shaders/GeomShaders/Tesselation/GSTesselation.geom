#version 430 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 12) out;

uniform float stretch = 0.7;

flat out vec4 color;

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;


void Emit(vec3 v, vec4 faceColor)
{
	gl_Position = mvpMatrix * vec4(v, 1.0);
	color = faceColor;
	EmitVertex();
}

void make_face (vec3 a, vec3 b, vec3 c)
{
	vec3 faceNormal = normalize(cross(c - a, c - b));
	vec4 faceColor = vec4 (1.0, 0.2, 0.4, 1.0) * (mat3(mvMatrix) * faceNormal).z;
	Emit(a, faceColor);
	Emit(b, faceColor);
	Emit(c, faceColor);

	EndPrimitive();
}

void main()
{
	int n;
	vec3 a = gl_in[0].gl_Position.xyz;
	vec3 b = gl_in[1].gl_Position.xyz;
	vec3 c = gl_in[2].gl_Position.xyz;

	vec3 d = (a + b) * stretch;
	vec3 e = (b + c) * stretch;
	vec3 f = (c + a) * stretch;

	a *= (2.0 - stretch);
	b *= (2.0 - stretch);
	c *= (2.0 - stretch);

	make_face(a, d, f);
	make_face(d, b, e);
	make_face(e, c, f);
	make_face(d, e, f);

	EndPrimitive();
}