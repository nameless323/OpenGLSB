#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"

class Textures : public sb6::application
{
public:
	void startup()
	{
		_renderingProgram = GetShaderProgram("Shaders/Textures/Texture.vert", "Shaders/Textures/Texture.frag");
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glGenBuffers(1, &_vertsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer);
		const GLfloat verts[] =
		{
			-0.5f, 0.5f, 0.5f, 1.0f,
			-0.5f, -0.5f, 0.5f, 1.0f,
			0.5f, -0.5f, 0.5f, 1.0f,

			-0.5f, 0.5f, 0.5f, 1.0f,
			0.5f, -0.5f, 0.5f, 1.0f,
			0.5f, 0.5f, 0.5f, 1.0f
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glUseProgram(_renderingProgram);
		glActiveTexture(GL_TEXTURE0 + 2);
		glGenTextures(1, &_texture);

		glBindTexture(GL_TEXTURE_2D, _texture);
		glTexStorage2D(GL_TEXTURE_2D, 8, GL_RGBA32F, 256, 256);
		float* data = new float[256*256*4];
		GenerateTexture(data, 256, 256);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGBA, GL_FLOAT, data);
		//glBindSampler(1, _texture);
		//glActiveTexture(_texture);
		//glActiveTexture(GL_TEXTURE0 + 0);
		glUniform1i(1, 2);

		delete[] data;
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
		//glClear(GL_DEPTH_BUFFER_BIT);
		glUseProgram(_renderingProgram);
		glDrawArrays(GL_TRIANGLES, 0, 6);


	}

	void shutdown()
	{
		glDeleteProgram(_renderingProgram);

		glDisableVertexAttribArray(0);

		glBindVertexArray(0);
		glDeleteVertexArrays(1, &_vao);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &_vertsBuffer);
		glDeleteTextures(1, &_texture);
	}

	void GenerateTexture(float * data, int width, int height)
	{
		int x, y;

		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				data[(y * width + x) * 4 + 0] = (float)((x & y) & 0xFF) / 255.0f;
				data[(y * width + x) * 4 + 1] = (float)((x | y) & 0xFF) / 255.0f;
				data[(y * width + x) * 4 + 2] = (float)((x ^ y) & 0xFF) / 255.0f;
				data[(y * width + x) * 4 + 3] = 1.0f;
			}
		}
	}

private:
	GLuint _renderingProgram;
	GLuint _vao;
	GLuint _vertsBuffer;
	GLuint _colorBuffer;
	GLuint _texture;

	vmath::mat4 projMatrix;
	GLfloat mvLocation = 2;
	GLfloat projLocation = 3;
	
};

DECLARE_MAIN(Textures);
