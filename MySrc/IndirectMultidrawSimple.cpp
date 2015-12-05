#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"

class IndirectMultidrawSimple : public sb6::application
{
public:
	void startup()
	{
		_renderingProgram = GetShaderProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		const GLfloat verts[] =
		{
			-0.2f, -0.2, 0.5f, //triangle
			-0.2f, 0.2f, 0.5f,
			0.2f, -0.2f, 0.5f,

			-1.0f, -1.0f, 0.5f, //fullscreen quad
			-1.0f, 1.0f, 0.5f,
			1.0f, -1.0f, 0.5f,
			1.0f, -1.0f, 0.5f,
			-1.0f, 1.0f, 0.5f,
			1.0f, 1.0f, 0.5f
		};

		float angle = vmath::radians(180.0f);
		float c = sinf(angle);
		float s = sinf(angle);
		const GLfloat mats[] =
		{
			1.0f, 0.0f, 0.0f, 0.0f, //trig 1
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			-0.2f, -0.2f, 0.0f, 1.0f,

			c, s, 0.0f, 0.0f, //trig 2
			-s, c, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.2f, 0.2f, 0.0f, 1.0f,

			0.1f, 0.0f, 0.0f, 0.0f, //cube 1
			0.0f, 0.1f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.1f, 0.0f,
			-0.8f, -0.8f, 0.0f, 1.0f,

			0.1f, 0.0f, 0.0f, 0.0f, //cube 2
			0.0f, 0.1f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.1f, 0.0f,
			-0.8f, 0.8f, 0.0f, 1.0f,

			0.1f, 0.0f, 0.0f, 0.0f, //cube 3
			0.0f, 0.1f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.1f, 0.0f,
			0.8f, 0.8f, 0.0f, 1.0f,

			0.1f, 0.0f, 0.0f, 0.0f, //cube 4
			0.0f, 0.1f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.1f, 0.0f,
			0.8f, -0.8f, 0.0f, 1.0f
		};
	}

	void onResize(int w, int h)
	{
		application::onResize(w, h);
	}

	void render(double currentTime)
	{

		const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, bckColor);
		glClearBufferfv(GL_DEPTH, 0, white);
		//		glClear(GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
	}

	void shutdown()
	{
		glDeleteProgram(_renderingProgram);

		glDisableVertexAttribArray(0);

		glBindVertexArray(0);
		glDeleteVertexArrays(1, &_vao);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &_vertsBuffer);
		glDeleteBuffers(1, &_elementsBuffer);
	}

private:
	GLuint _renderingProgram;
	GLuint _vao;
	GLuint _vertsBuffer;
	GLuint _elementsBuffer;

	vmath::mat4 projMatrix;
	GLfloat mvLocation = 2;
	GLfloat projLocation = 3;

};

DECLARE_MAIN(IndirectMultidrawSimple);

