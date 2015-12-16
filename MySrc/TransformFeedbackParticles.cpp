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

class TransformFeedbackParticles : public sb6::application
{
public:
	TransformFeedbackParticles() : _eye(3, 3, 3), _prevTime(0), _angle(0), _rot(0.0f, 0.0f, 0.0f), _mouseOldX(0), _mouseOldY(0), _ind(0)
	{}

	void LoadShaders()
	{
		GLuint vs, fs;
		vs = sb6::shader::load("Shaders/TransformFeedback/Particles.vert", GL_VERTEX_SHADER);
		fs = sb6::shader::load("Shaders/TransformFeedback/Particles.frag", GL_FRAGMENT_SHADER);
		static const char* feedbackVars[] =
		{
			"newPos",
			"newVel"
		};
		_shader = glCreateProgram();
		glAttachShader(_shader, vs);
		glAttachShader(_shader, fs);
		glTransformFeedbackVaryings(_shader, 2, feedbackVars, GL_SEPARATE_ATTRIBS);
		glLinkProgram(_shader);
		glDeleteShader(vs);
		glDeleteShader(fs);
	}

	void startup() override
	{
		glPointSize(3.0f);
		std::default_random_engine generator;
		std::uniform_real_distribution<float> distribution(-0.1f, 0.1f);
		for (int i = 0; i < NUM_PARTICLES; i++)
		{
			_pos[i] = vec3(0, 0, 0);
			_vel[i] = vec3(distribution(generator), distribution(generator), distribution(generator));
		}
		
		LoadShaders();

		glGenVertexArrays(2, _vao);
		glGenBuffers(2, _vertBuffer);
		glGenBuffers(2, _velBuffer);

		for (int i = 0; i < 2; i++)
		{
			glBindVertexArray(_vao[0]);
			glBindBuffer(GL_ARRAY_BUFFER, _vertBuffer[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * NUM_PARTICLES, _pos, GL_DYNAMIC_COPY);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

			glBindBuffer(GL_ARRAY_BUFFER, _velBuffer[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * NUM_PARTICLES, _vel, GL_DYNAMIC_COPY);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			
		}
		glUseProgram(_shader);
		_projLocation = glGetUniformLocation(_shader, "proj");
		_mvLocation = glGetUniformLocation(_shader, "mv");
		_dtLocation = glGetUniformLocation(_shader, "dt");
		_boxVecLocation = glGetUniformLocation(_shader, "boxSize");
		_projMatrix = vmath::perspective(60.0f, 1024.0f / 768.0f, 0.5f, 20.0f) * vmath::lookat(_eye, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(_projLocation, 1, GL_FALSE, _projMatrix);
		vec3 boxSize(2.0f, 2.0f, 2.0f);
		glUniform3fv(_boxVecLocation, 1, boxSize);
	}

	void onResize(int w, int h) override
	{
		application::onResize(w, h);
		
	}

	void render(double currentTime) override
	{
//		_projMatrix = vmath::perspective(60.0f, (float)w / (float)h, 0.5f, 20.0f) * vmath::lookat(_eye, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
//		glUniformMatrix4fv(_projLocation, 1, GL_FALSE, _projMatrix);
//		glViewport(0, 0, info.windowWidth, info.windowHeight);
		const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, bckColor);
		glClearBufferfv(GL_DEPTH, 0, white);

		_mv = rotate(vmath::radians(_rot[2]), vec3(0.0f, 0.0f, 1.0f)) *
			rotate(vmath::radians(_rot[0]), vec3(1.0f, 0.0f, 0.0f)) *
			rotate(vmath::radians(_rot[1]), vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(_mvLocation, 1, GL_FALSE, _mv);

		glUniform1f(_dtLocation, 0.025f);

		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _vertBuffer[_ind ^ 1]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, _velBuffer[_ind ^ 1]);

		glBeginTransformFeedback(GL_POINTS);
		glBindVertexArray(_vao[_ind]);
		glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
		glEndTransformFeedback();
		//glBindVertexArray(0);

		_prevTime = currentTime;
		_ind ^= 1;
	}

	void shutdown() override
	{
		glBindVertexArray(0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDeleteBuffers(2, _vertBuffer);
		glDeleteBuffers(2, _velBuffer);
		glDeleteProgram(_shader);
	}

	void onMouseMove(int x, int y) override
	{
		_rot[0] += ((_mouseOldY - y) * 180.0f) / 200.0f;
		_rot[2] += ((_mouseOldX - x) * 180.0f) / 200.0f;
		_rot[1] = 0.0f;

		if (_rot[2] > 360)
			_rot[2] -= 360;
		if (_rot[2] < -360)
			_rot[2] += 360;

		if (_rot[1] > 360)
			_rot[1] -= 360;
		if (_rot[1] < -360)
			_rot[1] += 360;

		_mouseOldX = x;
		_mouseOldY = y;
	}

private:
	GLuint _vao[2];
	GLuint _vertBuffer[2];
	GLuint _velBuffer[2];
	GLuint _shader;
	vec3 _pos[NUM_PARTICLES];
	vec3 _vel[NUM_PARTICLES];
	vec3 _eye;
	vec3 _rot;
	double _prevTime;
	int _mouseOldX;
	int _mouseOldY;
	float _angle;
	int _ind;
	mat4 _projMatrix;
	mat4 _mv;
	GLint _projLocation;
	GLint _mvLocation;
	GLint _boxVecLocation;
	GLint _dtLocation;
};

DECLARE_MAIN(TransformFeedbackParticles);

