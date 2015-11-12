#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"

class SeparateShaderPipeline : public sb6::application
{
public:
	void startup()
	{
		_renderingProgram = GetShaderProgram("Shaders/SeparateShaderPipeline/ShaderV.vert", "Shaders/SeparateShaderPipeline/ShaderFBlue.frag");
		glGenVertexArrays(2, _vao);
		glBindVertexArray(_vao[0]);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glGenBuffers(2, _vertsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer[0]);
		const GLfloat verts1[] =
		{
			-1.0f, -1.0f, 0.5f, 1.0f,
			0.0f, -1.0f, 0.5f, 1.0f,
			-1.0f, 0.0f, 0.5f, 1.0f,
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts1), verts1, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glBindVertexArray(_vao[1]);
		glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer[1]);
		const GLfloat verts2[] =
		{
			0.0f, 1.0f, 0.5f, 1.0f,
			1.0f, 1.0f, 0.5f, 1.0f,
			1.0f, 0.0f, 0.5f, 1.0f,
			0.0f, 0.0f, 0.5f, 1.0f

		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts2), verts2, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &_elementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementBuffer);
		GLuint indices[] = { 0, 1, 2, 2, 3, 0 };
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementBuffer);

		glUseProgram(_renderingProgram);
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
		glBindVertexArray(_vao[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(_vao[1]);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}

	void shutdown()
	{
		glDeleteProgram(_renderingProgram);

		glDisableVertexAttribArray(0);

		glBindVertexArray(0);
		glDeleteVertexArrays(2, _vao);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(2, _vertsBuffer);

		glDeleteBuffers(1, &_elementBuffer);
	}

private:
	GLuint _renderingProgram;
	GLuint _vao[2];
	GLuint _vertsBuffer[2];
	GLuint _elementBuffer;

	vmath::mat4 projMatrix;
	GLfloat mvLocation = 2;
	GLfloat projLocation = 3;

};

DECLARE_MAIN(SeparateShaderPipeline);

