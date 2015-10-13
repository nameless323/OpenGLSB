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
		const float vertices[] =
			{
				0.25f , -0.25f , 0.5f , 1.0f ,
				-0.25f , -0.25f , 0.5f , 1.0f ,
				0.25f , 0.25f , 0.5f , 1.0f
			};
		glGenBuffers(1, &_vertsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer);
		glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), nullptr, GL_STATIC_DRAW);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(5);

		const float color[] = {0.0f, 1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f };
		glGenBuffers(1, &_colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(1);

		//glPointSize(40.0f);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void render(double currentTime)
	{
		//const GLfloat bckColor[] = {sin(currentTime) * 0.5f + 0.5f, cos(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f};
		const GLfloat bckColor[] = {0.0f, 0.0f, 0.0f, 1.0f};

		glClearBufferfv(GL_COLOR, 0, bckColor);
		glUseProgram(_renderingProgram);
		//GLfloat offset[] = { sin(currentTime) * 0.5f, cos(currentTime) * 0.5f, 0.0f, 0.0f };
		GLfloat offset[] = {0.0f, 0.0f, 0.0f, 0.0f};

		//GLfloat color[] = { cos(currentTime) * 0.5f + 0.5f, sin(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f };
		GLfloat color[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glVertexAttrib4fv(0, offset);
		glVertexAttrib4fv(1, color);
		const float vertices[] =
			{
				0.25f * cosf(currentTime), -0.25f , 0.5f , 1.0f ,
				-0.25f , -0.25f , 0.5f , 1.0f ,
				0.25f , 0.25f , 0.5f , 1.0f
			};
		//glClearBufferSubData(GL_ARRAY_BUFFER, GL_FLOAT, 0, sizeof(vertices), GL_RGBA, GL_FLOAT, vertices);
		glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 12 * sizeof(float), vertices);
		//const float newVert[] = {-0.25f, 0.25f, 0.5f, 1.0f};
		//glBufferSubData(GL_ARRAY_BUFFER, 12 * sizeof(float) - sizeof(newVert), sizeof(newVert), newVert);
		//glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		//glUseProgram(0);
	}

	void shutdown()
	{
		glDeleteProgram(_renderingProgram);

		glDisableVertexAttribArray(5);
		glDisableVertexAttribArray(0);

		glBindVertexArray(0);
		glDeleteVertexArrays(1, &_vao);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &_vertsBuffer);
	}

private:
	GLuint _renderingProgram;
	GLuint _vao;
	GLuint _vertsBuffer;
	GLuint _colorBuffer;
};

DECLARE_MAIN(Ch5Data);

