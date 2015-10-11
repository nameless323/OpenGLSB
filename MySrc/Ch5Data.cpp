#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>

GLuint CompileShaders1()
{
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;

	vertexShader = sb6::shader::load("Shaders/Ch5Data.vert", GL_VERTEX_SHADER);
	fragmentShader = sb6::shader::load("Shaders/Ch5Data.frag", GL_FRAGMENT_SHADER);

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

class Ch5Data : public sb6::application
{
public:
	void startup()
	{
		_renderingProgram = CompileShaders1();
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		//glPointSize(40.0f);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	void render(double currentTime)
	{
		//const GLfloat bckColor[] = {sin(currentTime) * 0.5f + 0.5f, cos(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f};
		const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		glClearBufferfv(GL_COLOR, 0, bckColor);
		glUseProgram(_renderingProgram);
		//GLfloat offset[] = { sin(currentTime) * 0.5f, cos(currentTime) * 0.5f, 0.0f, 0.0f };
		GLfloat offset[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		//GLfloat color[] = { cos(currentTime) * 0.5f + 0.5f, sin(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f };
		GLfloat color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glVertexAttrib4fv(0, offset);
		glVertexAttrib4fv(1, color);

		glDrawArrays(GL_TRIANGLES, 0, 3);
		//glUseProgram(0);
	}
	void shutdown()
	{
		glDeleteProgram(_renderingProgram);
		glDeleteVertexArrays(1, &_vao);
	}
private:
	GLuint _renderingProgram;
	GLuint _vao;
};

DECLARE_MAIN(Ch5Data);