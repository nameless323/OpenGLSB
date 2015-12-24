#version 430 core

uniform mat4 proj;
uniform mat4 mv;
uniform float dt;
uniform vec3 boxSize;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 vel;

out vec3 newPos;
out vec3 newVel;

void main(void)
{
	const vec3 acc = vec3(0, -0.02, 0); //gravity
	const float damp = 0.9;
	bool refl = false;

	gl_Position = proj * mv * vec4(pos, 1.0);
	newPos = pos + dt * vel;
	newVel = vel + dt * acc;

	if (abs(newPos.x) >= boxSize.x)
	{
		newPos -= dt * vel;
		newVel.x = -newVel.x;
		refl = true;
	}

	if (abs (newPos.y) >= boxSize.y)
	{
		newPos -= dt * vel;
		newVel.y = -newVel.y;
		refl = true;
	}

	if (abs (newPos.z) >= boxSize.z)
	{
		newPos -= dt * vel;
		newVel.z = -newVel.z;
		refl = true;
	}

	if (refl)
		newVel *= damp;
}