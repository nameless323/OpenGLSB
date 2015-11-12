#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"

GLuint CreateShader(const char* filename, GLenum type)
{
	GLuint shader = glCreateShader(type);
	FILE * fp;
	size_t filesize;
	char * data;

	fp = fopen(filename, "rb");
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	data = new char[filesize + 1];
	fread(data, 1, filesize, fp);
	data[filesize] = 0;
	fclose(fp);

	glShaderSource(shader, 1, &data, nullptr);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		OutputDebugStringA("---------------\n");
		OutputDebugStringA("Shader Error:\n");
		GLint len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		char* log = new char[len + 1];

		GLsizei byteWritten;
		glGetShaderInfoLog(shader, len, &byteWritten, log);
		log[len] = '\0';
		OutputDebugStringA(log);
		OutputDebugStringA("---------------\n");
		delete[] log;
	}
	delete[] data;
	return shader;
}

class SeparateShaderPipeline : public sb6::application
{
public:
	void startup()
	{
		_vertProgram = glCreateProgram();
		_vert = CreateShader("Shaders/SeparateShaderPipeline/ShaderV.vert", GL_VERTEX_SHADER);
		_fragBlue = CreateShader("Shaders/SeparateShaderPipeline/ShaderFBlue.frag", GL_FRAGMENT_SHADER);
		glAttachShader(_vertProgram, _vert);
		glProgramParameteri(_vertProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);
		glLinkProgram(_vertProgram);
		glDeleteShader(_vert);
		GLint status;
		glGetProgramiv(_vertProgram, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint len;
			glGetProgramiv(_vertProgram, GL_INFO_LOG_LENGTH, &len);
			char* log = new char[len + 1];
			GLsizei bytesWritten;
			glGetProgramInfoLog(_vertProgram, len, &bytesWritten, log);
			log[len] = '\0';

			OutputDebugStringA("-------------------\nShader program linking error:\n");
			OutputDebugStringA(log);
			OutputDebugStringA("--------------\n");
			delete[] log;
		}

		_fragProgramBlue = glCreateProgram();
		glAttachShader(_fragProgramBlue, _fragBlue);
		glProgramParameteri(_fragProgramBlue, GL_PROGRAM_SEPARABLE, GL_TRUE);
		glLinkProgram(_fragProgramBlue);
		glDeleteShader(_fragBlue);

		FILE * fp;
		size_t filesize;
		char * data;

		fp = fopen("Shaders/SeparateShaderPipeline/ShaderFGreen.frag", "rb");
		fseek(fp, 0, SEEK_END);
		filesize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		data = new char[filesize + 1];
		fread(data, 1, filesize, fp);
		data[filesize] = 0;
		fclose(fp);
		_fragProgramGreen = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &data);

		delete[] data;

		glGenProgramPipelines(2, _programPiplines);
		glUseProgramStages(_programPiplines[0], GL_VERTEX_SHADER_BIT, _vertProgram);
		glUseProgramStages(_programPiplines[0], GL_FRAGMENT_SHADER_BIT, _fragProgramBlue);

		glUseProgramStages(_programPiplines[1], GL_VERTEX_SHADER_BIT, _vertProgram);
		glUseProgramStages(_programPiplines[1], GL_FRAGMENT_SHADER_BIT, _fragProgramGreen);

		glGenVertexArrays(2, _vao);
		glBindVertexArray(_vao[0]);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glGenBuffers(2, _vertsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer[0]);
		const GLfloat verts1[] =
		{
			-1.0f, -1.0f, 0.5f, 1.0f,
			0.0f, -1.0f, 0.5f, 1.0f,
			-1.0f, 0.0f, 0.5f, 1.0f,
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts1), verts1, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glBindVertexArray(_vao[1]);
		glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer[1]);
		const GLfloat verts2[] =
		{
			0.0f, 1.0f, 0.5f, 1.0f,
			1.0f, 1.0f, 0.5f, 1.0f,
			1.0f, 0.0f, 0.5f, 1.0f,
			0.0f, 0.0f, 0.5f, 1.0f

		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts2), verts2, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &_elementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementBuffer);
		GLuint indices[] = { 0, 1, 2, 2, 3, 0 };
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
		glBindVertexArray(_vao[0]);
		glBindProgramPipeline(_programPiplines[0]);

		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(_vao[1]);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindProgramPipeline(_programPiplines[1]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}

	void shutdown()
	{
		glUseProgram(0);
		glBindProgramPipeline(0);
		glDeleteProgramPipelines(2, _programPiplines);
		glDeleteProgram(_vertProgram);
		glDeleteProgram(_fragProgramBlue);
		glDeleteProgram(_fragProgramGreen);

		glDisableVertexAttribArray(0);

		glBindVertexArray(0);
		glDeleteVertexArrays(2, _vao);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(2, _vertsBuffer);

		glDeleteBuffers(1, &_elementBuffer);
	}

private:
	GLuint _vertProgram;
	GLuint _fragProgramBlue;
	GLuint _fragProgramGreen;
	GLuint _programPiplines[2];
	GLuint _vao[2];
	GLuint _vertsBuffer[2];
	GLuint _elementBuffer;

	GLuint _vert;
	GLuint _fragGreen;
	GLuint _fragBlue;

	vmath::mat4 projMatrix;
	GLfloat mvLocation = 2;
	GLfloat projLocation = 3;

};

//DECLARE_MAIN(SeparateShaderPipeline);

