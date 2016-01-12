#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"
#include "ShaderProgram.h"

class Dummy : public sb6::application
{
public:
	void startup()
	{
//		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
		_renderingProgram.CreateProgram();
		_renderingProgram.AttachShader("Shaders/Common/Default.vert");
		_renderingProgram.AttachShader("Shaders/Common/Default.frag");
		_renderingProgram.Link();
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
			0.5f, 0.5f, 0.5f,
			0.5f, -0.5f, 0.5f
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		const GLubyte indices[] =
		{
			0, 1, 2,
			0, 2, 3
		};
		glGenBuffers(1, &_elementsBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementsBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		_renderingProgram.Use();
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
		_renderingProgram.DeleteProgram();

		glDisableVertexAttribArray(0);

		glBindVertexArray(0);
		glDeleteVertexArrays(1, &_vao);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &_vertsBuffer);
		glDeleteBuffers(1, &_elementsBuffer);
	}

private:
	ShaderProgram _renderingProgram;
	GLuint _vao;
	GLuint _vertsBuffer;
	GLuint _elementsBuffer;

	vmath::mat4 projMatrix;
	GLfloat mvLocation = 2;
	GLfloat projLocation = 3;

};

//DECLARE_MAIN(Dummy);

