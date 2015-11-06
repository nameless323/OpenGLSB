#include <sb6.h>
#include <sb6ktx.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"

static unsigned int seed = 0x13371337;

static float random_float()
{
	float res;
	unsigned int tmp;

	seed *= 16807;

	tmp = seed ^ (seed >> 4) ^ (seed << 15);

	*((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

	return (res - 1.0f);
}

class AlienRain : public sb6::application
{
public:
	void startup()
	{
		_renderingProgram = GetShaderProgram("Shaders/AlienRain/AlienRain.vert", "Shaders/AlienRain/AlienRain.frag");
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		_alienTex = sb6::ktx::file::load("media/textures/aliens.ktx");
		GLuint textureUnit = 0;
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D_ARRAY, _alienTex);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glGenBuffers(1, &_rainBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _rainBuffer);
		glBufferData(GL_UNIFORM_BUFFER, 256 * sizeof(vmath::vec4), NULL, GL_DYNAMIC_DRAW);

		for (int i = 0; i < 256; i++)
		{
			dropletXOffset[i] = random_float() * 2.0f - 1.0f;
			dropletRotSpeed[i] = (random_float() + 0.5f) * ((i & 1) ? -3.0f : 3.0f);
			dropletFallSpeed[i] = random_float() + 0.2f;
		}

		glBindVertexArray(_vao); //?
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void onResize(int w, int h)
	{
		application::onResize(w, h);
	}

	void render(double currentTime)
	{

		glViewport(0, 0, info.windowWidth, info.windowHeight);
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
		glDisableVertexAttribArray(4);

		glBindVertexArray(0);
		glDeleteVertexArrays(1, &_vao);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &_rainBuffer);
		glDeleteBuffers(1, &_uvBuffer);
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
	GLuint _rainBuffer;
	GLuint _uvBuffer;
	GLuint _sampler;
	GLuint _alienTex;

	float dropletXOffset[256];
	float dropletRotSpeed[256];
	float dropletFallSpeed[256];

	vmath::mat4 projMatrix;
	GLfloat mvLocation = 2;
	GLfloat projLocation = 3;

};

DECLARE_MAIN(AlienRain);
