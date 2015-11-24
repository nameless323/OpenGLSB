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

	GLint linkStatus;

	
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		GLint len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
		char* log = new char[len + 1];
		GLsizei bytesWritten;
		glGetProgramInfoLog(program, len, &bytesWritten, log);
		log[len] = '\0';

		OutputDebugStringA("-------------------\nShader program linking error:\n");
		OutputDebugStringA(log);
		OutputDebugStringA("--------------\n");
		delete[] log;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

//inline void Log(float arg)
//{
//	std::wstringstream wss;
//	wss << arg << std::endl;
//	OutputDebugString(wss.str().c_str());
//}
