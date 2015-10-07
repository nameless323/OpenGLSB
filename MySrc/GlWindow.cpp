#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>

void Log(float arg)
{
	std::wstringstream wss;
	wss << arg << std::endl;
	OutputDebugString(wss.str().c_str());
}

GLuint CompileShaders()
{
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;

	static const GLchar* vertexShaderSource[] = 
	{
		"#version 430 core \n"
		"\n"
		"void main(void) \n"
		"{ \n"
		"	gl_Position = vec4(0.0, 0.0, 0.5, 1.0); \n"
		"} \n" 
	};
	static const GLchar* fragmentShaderSource[] =
	{
		"#version 430 core \n"
		"out vec4 color; \n"
		"void main(void) \n"
		"{ \n"
		"	color = vec4(0.0, 0.8, 1.0, 1.0); \n"
		"} \n"
	};

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

class GlWindow : public sb6::application
{
public:
	void render(double currentTime)
	{
		const GLfloat red[] = {sin(currentTime) * 0.5f + 0.5f, cos(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f};
		float val = red[0];
		Log(val);
		glClearBufferfv(GL_COLOR, 0, red);
	}
};

DECLARE_MAIN(GlWindow);

