#pragma once
#include <GL/gl3w.h>
#include <shader.h>

inline GLuint GetShaderProgram(const char* vertName, const char* fragName)
{
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;

	vertexShader = sb6::shader::load(vertName, GL_VERTEX_SHADER);
	fragmentShader = sb6::shader::load(fragName, GL_FRAGMENT_SHADER);

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}
