#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"

class Cubes : public sb6::application
{
public:
	void startup()
	{
		_renderingProgram = GetShaderProgram("Shaders/Cubes/Cubes.vert", "Shaders/Cubes/Cubes.frag");
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		static const GLfloat positions[] =
		{
			-0.25f, 0.25f, -0.25f,
			-0.25f, -0.25f, -0.25f,
			0.25f, -0.25f, -0.25f,

			0.25f, -0.25f, -0.25f,
			0.25f, 0.25f, -0.25f,
			-0.25f, 0.25f, -0.25f,

			-0.25f, 0.25f, -0.25f,
			0.25f, 0.25f, -0.25f,
			0.25f, 0.25f, 0.25f,

			0.25f, 0.25f, 0.25f,
			-0.25f, 0.25f, 0.25f,
			-0.25f, 0.25f, -0.25f
		};

		glGenBuffers(1, &_vertsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);
		mvLocation = glGetUniformLocation(_renderingProgram, "mvMat");
		projLocation = glGetUniformLocation(_renderingProgram, "projMat");

		glUseProgram(_renderingProgram);
	}

	void onResize(int w, int h)
	{
		application::onResize(w, h);
		float aspect = (float)info.windowWidth / (float)info.windowHeight;
		projMatrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);
	}

	void render(double currentTime)
	{
		float f = (float)currentTime * (float)M_PI * 0.1f;
		vmath::mat4 mvMatrix =
			vmath::translate(0.0f, 0.0f, -4.0f) *
			vmath::translate(sinf(2.1f * f)*0.5f,
				cosf(1.7f * f) * 0.5f,
				sinf(1.3f*f) * cosf(1.5f * f) * 2.0f)*
			vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);

		const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		glClearBufferfv(GL_COLOR, 0, bckColor);
		
		glUniformMatrix4fv(mvLocation, 1, GL_FALSE, mvMatrix);
		glUniformMatrix4fv(projLocation, 1, GL_FALSE, projMatrix);
		
		
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void shutdown()
	{
		glDeleteProgram(_renderingProgram);

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

	vmath::mat4 projMatrix;
	GLfloat mvLocation = 2;
	GLfloat projLocation = 3;

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

