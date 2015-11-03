#include <sb6.h>
#include <sb6ktx.h>
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
			-1.0f, 1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 1.0f,

			-1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &_uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _uvBuffer);
		GLfloat uv[] = 
		{
			0.0f, 1.0f,
			0.0f, 0.0f,
			2.0f, 0.0f,
			0.0f, 1.0f,
			2.0f, 0.0f,
			2.0f, 1.0f
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);
//		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glBindVertexBuffer(1, _uvBuffer, 0, 2 * sizeof(GLfloat));
		glVertexAttribFormat(4, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexAttribBinding(4, 1);
		glEnableVertexAttribArray(4);
		
		glUseProgram(_renderingProgram);

		int texureUnit = 0;
		int texureUnit2 = 1;
		glActiveTexture(GL_TEXTURE0 + texureUnit);
		glGenTextures(1, &_texture);
		sb6::ktx::file::load("media/textures/tree.ktx", _texture);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);


//		glBindTexture(GL_TEXTURE_2D, _texture);
//		glTexStorage2D(GL_TEXTURE_2D, 8, GL_RGBA32F, 256, 256);
//		float* data = new float[256*256*4];
//		GenerateTexture(data, 256, 256);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glGenSamplers(1, &_sampler);
		const GLfloat border[] = {1.0f, 0.0f, 0.0f, 1.0f};
		glSamplerParameterfv(_sampler, GL_TEXTURE_BORDER_COLOR, border);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glSamplerParameterf(_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);



		glBindSampler(texureUnit, _sampler);

		glActiveTexture(GL_TEXTURE0 + texureUnit2);
		glGenTextures(1, &_texture2);
		sb6::ktx::file::load("media/textures/rightarrows.ktx", _texture2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGBA, GL_FLOAT, data);
		//glBindSampler(1, _texture);
		//glActiveTexture(_texture);
		//glActiveTexture(GL_TEXTURE0 + 0);
//		glUniform1i(1, texureUnit);
//		glUniform1i(2, texureUnit2);

		
//		delete[] data;
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
		glUniform1f(3, (sinf(currentTime) + 1.0f)*0.5f);
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
		glDeleteBuffers(1, &_vertsBuffer);
		glDeleteBuffers(1, &_uvBuffer);
		glDeleteTextures(1, &_texture);
		glDeleteTextures(1, &_texture2);

		glBindSampler(GL_TEXTURE0, 0);
		glDeleteSamplers(1, &_sampler);
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
	GLuint _uvBuffer;
	GLuint _texture;
	GLuint _texture2;
	GLuint _sampler;

	vmath::mat4 projMatrix;
	GLfloat mvLocation = 2;
	GLfloat projLocation = 3;
	
};

DECLARE_MAIN(Textures);
