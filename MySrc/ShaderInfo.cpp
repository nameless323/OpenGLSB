#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"

class ShaderInfo : public sb6::application
{
public:
	void startup()
	{
		GLuint vert = glCreateShader(GL_VERTEX_SHADER);
		FILE * fp;
		size_t filesize;
		char * data;

		fp = fopen("Shaders/ShaderInfo/ShaderV.vert", "rb");
		fseek(fp, 0, SEEK_END);
		filesize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		data = new char[filesize + 1];
		fread(data, 1, filesize, fp);
		data[filesize] = 0;
		fclose(fp);

		glShaderSource(vert, 1, &data, nullptr);
		glCompileShader(vert);
		delete[] data;

		GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
		fp = fopen("Shaders/ShaderInfo/ShaderF.frag", "rb");
		fseek(fp, 0, SEEK_END);
		filesize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		data = new char[filesize + 1];
		fread(data, 1, filesize, fp);
		data[filesize] = 0;
		fclose(fp);

		glShaderSource(frag, 1, &data, nullptr);
		glCompileShader(frag);
		delete[] data;

		_renderingProgram = glCreateProgram();
		glAttachShader(_renderingProgram, vert);
		glAttachShader(_renderingProgram, frag);
		glLinkProgram(_renderingProgram);
		glDeleteShader(vert);

		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glGenBuffers(1, &_vertsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer);
		const GLfloat verts1[] =
		{
			-0.35f, 0.25f, 0.5f, 1.0f,
			0.15f, -0.25f, 0.5f, 1.0f,
			-0.35f, -0.25f, 0.5f, 1.0f,
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts1), verts1, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

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

		glDrawArrays(GL_TRIANGLES, 0, 3);


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

};

DECLARE_MAIN(ShaderInfo);

