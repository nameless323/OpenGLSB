#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include "ShaderUtils.h"

class Uniforms : public sb6::application
{
public:
	void startup()
	{
		_renderingProgram1 = GetShaderProgram("Shaders/Uniforms/Uniforms1.vert", "Shaders/Uniforms/Uniforms1.frag");
		_renderingProgram2 = GetShaderProgram("Shaders/Uniforms/Uniforms2.vert", "Shaders/Uniforms/Uniforms2.frag");
		glGenVertexArrays(1, &_vao1);
		glBindVertexArray(_vao1);

		glGenBuffers(1, &_vertsBuffer1);
		glBindBuffer(GL_ARRAY_BUFFER, _vao1);

		const GLfloat verts1[] =
		{
			-0.35f, 0.25f, 0.5f, 1.0f,
			0.15f, -0.25f, 0.5f, 1.0f,
			-0.35f, -0.25f, 0.5f, 1.0f,
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts1), verts1, GL_STATIC_DRAW);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(5);
	}

	void render(double currentTime)
	{
		const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		glClearBufferfv(GL_COLOR, 0, bckColor);
		glUseProgram(_renderingProgram1);
		
		const GLfloat color[] = { 1.0f, 1.0f, 0.0f, 1.0f };
		glUniform4fv(0, 1, color);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	void shutdown()
	{
		glDeleteProgram(_renderingProgram1);

		glDisableVertexAttribArray(5);

		glBindVertexArray(0);
		glDeleteVertexArrays(1, &_vao1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &_vertsBuffer1);
	}

private:
	GLuint _renderingProgram1;
	GLuint _renderingProgram2;
	GLuint _vao1;
	GLuint _vao2;
	GLuint _vertsBuffer1;
	GLuint _vertsBuffer2;

};

DECLARE_MAIN(Uniforms);

