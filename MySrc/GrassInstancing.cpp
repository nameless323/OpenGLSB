#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include <sb6ktx.h>
#include "vmath.h"

class GrassInstancing : public sb6::application
{
public:
	void startup()
	{
		static const GLfloat grassBlade[] = 
		{
			-0.3f, 0.0f,
			0.3f, 0.0f,
			-0.2f, 1.0f,
			0.1f, 1.3f,
			-0.05f, 2.3f,
			0.0f, 3.3f
		};
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		
		glGenBuffers(1, &_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, _buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(grassBlade), grassBlade, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		
		_shaderProgram = GetShaderProgram("Shaders/GrassInstancing/GrassInstancing.frag", "Shaders/GrassInstancing/GrassInstancing.vert");

		uniforms.mvpMatrix = glGetUniformLocation(_shaderProgram, "mvpMatrix");

		glActiveTexture(GL_TEXTURE0 + 1);
		_texGrassLen = sb6::ktx::file::load("media/textures/grass_length.ktx");

		glActiveTexture(GL_TEXTURE0 + 2);
		_texGrassLen = sb6::ktx::file::load("media/textures/grass_orientation.ktx");

		glActiveTexture(GL_TEXTURE0 + 3);
		_texGrassLen = sb6::ktx::file::load("media/textures/grass_color.ktx");

		glActiveTexture(GL_TEXTURE0 + 4);
		_texGrassLen = sb6::ktx::file::load("media/textures/grass_bend.ktx");
	}

private:
	GLuint _buffer;
	GLuint _vao;

	GLuint _shaderProgram;

	GLuint _texGrassColor;
	GLuint _texGrassLen;
	GLuint _texGrassOrientation;
	GLuint _texGrassBend;

	struct
	{
		GLint mvpMatrix;
	} uniforms;
};

DECLARE_MAIN(GrassInstancing);