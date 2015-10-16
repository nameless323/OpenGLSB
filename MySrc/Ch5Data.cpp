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

		const Vertex verts[] =
		{
			Vertex(0.25f , -0.25f , 0.5f , 1.0f , 0.0f, 1.0f, 0.0f, 1.0f),
			Vertex(-0.25f , -0.25f , 0.5f , 1.0f , 1.0f, 0.0f, 0.0f, 1.0f),
			Vertex(0.25f , 0.25f , 0.5f , 1.0f , 0.0f, 0.0f, 1.0f, 1.0f),
		};
		glGenBuffers(1, &_vertsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(4 * sizeof(float)));
		glEnableVertexAttribArray(1);
		
	}

	void render(double currentTime)
	{
		const GLfloat bckColor[] = {0.0f, 0.0f, 0.0f, 1.0f};

		glClearBufferfv(GL_COLOR, 0, bckColor);
		glUseProgram(_renderingProgram);
		GLfloat offset[] = {0.0f, 0.0f, 0.0f, 0.0f};

		GLfloat color[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glVertexAttrib4fv(0, offset);
		glVertexAttrib4fv(1, color);
		const float vertices[] =
			{
				0.25f * cosf(currentTime), -0.25f , 0.5f , 1.0f ,
				-0.25f , -0.25f , 0.5f , 1.0f ,
				0.25f , 0.25f , 0.5f , 1.0f
			};
		glUniform4f(2, 1.0f, 1.0f, 0.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, 3);
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
	struct Vertex
	{
		float X, Y, Z, W;
		float R, G, B, A;

		Vertex(float x, float y, float z, float w, float r, float g, float b, float a) : X(x), Y(y), Z(z), W(w), R(r), G(g), B(b), A(a) 
		{}
	};
};

DECLARE_MAIN(Ch5Data);

