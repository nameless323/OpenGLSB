#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"
#include <random>

using vmath::vec3;
using vmath::mat4;

#define NUM_PARTICLES 5000

class Dummy : public sb6::application
{
public:
	void startup() override
	{
		std::default_random_engine generator;
		std::uniform_real_distribution<float> distribution(-0.1f, 0.1f);
		for (int i = 0; i < NUM_PARTICLES; i++)
		{
			pos[i] = vec3(0, 0, 0);
			vel[i] = vec3(distribution(generator), distribution(generator), distribution(generator));
		}
	}

	void onResize(int w, int h) override
	{
		application::onResize(w, h);
	}

	void render(double currentTime) override
	{

		const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, bckColor);
		glClearBufferfv(GL_DEPTH, 0, white);
	}

	void shutdown() override
	{
	}

private:
	vec3 pos[NUM_PARTICLES];
	vec3 vel[NUM_PARTICLES];
	mat4 projMatrix;
};

//DECLARE_MAIN(Dummy);

