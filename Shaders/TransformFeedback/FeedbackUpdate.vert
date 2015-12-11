#version 430 core

layout (location = 0) in vec4 positionMass;
layout (location = 1) in vec3 velocity;
layout (location = 2) in ivec4 connection;

uniform samplerBuffer texPosition;

out vec4 tf_position_mass;
out vec3 tf_velocity;

uniform float t = 0.07;

uniform float k = 7.1;

const vec3 gravity = vec3(0.0, -0.08, 0.0);
uniform float c = 2.8;

uniform float restLength = 0.88;

void main()
{
	vec3 p = positionMass.xyz;
	float m = positionMass.w;
	vec3 u = velocity;
	vec3 f = gravity * m - c * u;
	bool fixedNode = true;

	for (int i = 0; i < 4; i++)
	{
		if (connection[i] != -1)
		{
			vec3 q = texelFetch(texPosition, connection[i]).xyz;
			vec3 d = q - p;
			float x = length(d);
			f += -k * (restLength - x) * normalize(d);
			fixedNode = false;
		}
	}

	if (fixedNode)
		f = vec3(0.0);

	vec3 a = f/m;
	vec3 s = u * t + 0.5 * a * t * t;
	vec3 v = u + a * t;

	s = clamp(s, vec3(-25.0), vec3(25.0));
	tf_position_mass = vec4(p + s, m);
	tf_velocity = v;
}