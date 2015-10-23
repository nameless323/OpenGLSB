#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"

class Cubes : public sb6::application
{
public:
	void startup()
	{
		_renderingProgram = GetShaderProgram("Shaders/Cubes/Cubes.vert", "Shaders/Cubes/Cubes.frag");
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		const Vertex verts[] =
		{
			Vertex(0.25f, -0.25f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f),
			Vertex(-0.25f, -0.25f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
			Vertex(0.25f, 0.25f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f),
		};
		glGenBuffers(1, &_vertsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(4 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	void render(double currentTime)
	{
		const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		glClearBufferfv(GL_COLOR, 0, bckColor);
		glUseProgram(_renderingProgram);
		
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	void shutdown()
	{
		glDeleteProgram(_renderingProgram);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

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
		{
		}
	};
};

DECLARE_MAIN(Cubes);

