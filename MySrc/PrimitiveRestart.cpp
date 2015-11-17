#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"

class PrimitiveRestart : public sb6::application
{
public:
	void startup()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		_renderingProgram = GetShaderProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glGenBuffers(1, &_vertsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer);
		const GLfloat verts[] =
		{
			-0.5f, -0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,
			-0.4f, -0.5f, 0.5f,
			-0.4f, 0.5f, 0.5f,
			-0.3f, -0.5f, 0.5f,
			-0.3f, 0.5f, 0.5f,
			-0.2f, -0.5f, 0.5f,
			-0.2f, 0.5f, 0.5f,
			-0.1f, -0.5f, 0.5f,
			-0.1f, 0.5f, 0.5f,
			0.0f, -0.5f, 0.5f,
			0.0f, 0.5f, 0.5f,
			0.1f, -0.5f, 0.5f,
			0.1f, 0.5f, 0.5f,
			0.2f, -0.5f, 0.5f,
			0.2f, 0.5f, 0.5f,
			0.3f, -0.5f, 0.5f,
			0.3f, 0.5f, 0.5f,
			0.4f, -0.5f, 0.5f,
			0.4f, 0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		const GLubyte indices[] =
		{
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21
		};
		glGenBuffers(1, &_elementsBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementsBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glUseProgram(_renderingProgram);

	}

	void onResize(int w, int h)
	{
		application::onResize(w, h);
	}

	void render(double currentTime)
	{

		glEnable(GL_PRIMITIVE_RESTART);

		glPrimitiveRestartIndex((int)currentTime % 21);
		const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, bckColor);
		glClearBufferfv(GL_DEPTH, 0, white);
		//		glClear(GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLE_STRIP, 22, GL_UNSIGNED_BYTE, nullptr);
		glDisable(GL_PRIMITIVE_RESTART);
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

//DECLARE_MAIN(PrimitiveRestart);

