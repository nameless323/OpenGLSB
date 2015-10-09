#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>

void Log(float arg)
{
	std::wstringstream wss;
	wss << arg << std::endl;
	OutputDebugString(wss.str().c_str());
}

GLuint load(const char * filename, GLenum shader_type, bool check_errors = true)
{
	
	GLuint result = 0;
	FILE * fp;
	size_t filesize;
	char * data;

	fp = fopen(filename, "rb");

	if (!fp)
	{
		OutputDebugStringA("::::::::::::::::::::::::::::::lool\n");
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	data = new char[filesize + 1];

	if (!data)
		goto fail_data_alloc;

	fread(data, 1, filesize, fp);
	data[filesize] = 0;
	fclose(fp);

	result = glCreateShader(shader_type);

	if (!result)
		goto fail_shader_alloc;

	glShaderSource(result, 1, &data, NULL);

	delete[] data;

	glCompileShader(result);

	if (check_errors)
	{
		GLint status = 0;
		glGetShaderiv(result, GL_COMPILE_STATUS, &status);

		if (!status)
		{
			char buffer[4096];
			glGetShaderInfoLog(result, 4096, NULL, buffer);
#ifdef _WIN32
			OutputDebugStringA(filename);
			OutputDebugStringA(":");
			OutputDebugStringA(buffer);
			OutputDebugStringA("\n");
#else
			fprintf(stderr, "%s: %s\n", filename, buffer);
#endif
			goto fail_compile_shader;
		}
	}

	return result;

fail_compile_shader:
	glDeleteShader(result);

fail_shader_alloc:;
fail_data_alloc:
	return result;
}

GLuint CompileShaders()
{
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint tesControlShader;
	GLuint tesEvalShader;
	GLuint geomShader;
	GLuint program;


	vertexShader = sb6::shader::load("Shaders/Shader1.vert", GL_VERTEX_SHADER);
	fragmentShader = sb6::shader::load("Shaders/Shader1.frag", GL_FRAGMENT_SHADER);
	tesControlShader = sb6::shader::load("Shaders/Shader1.tesc", GL_TESS_CONTROL_SHADER);
	tesEvalShader = sb6::shader::load("Shaders/Shader1.tese", GL_TESS_EVALUATION_SHADER);
	geomShader = sb6::shader::load("Shaders/Shader1.geom", GL_GEOMETRY_SHADER);

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glAttachShader(program, tesControlShader);
	glAttachShader(program, tesEvalShader);
	glAttachShader(program, geomShader);
	glLinkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(tesControlShader);
	glDeleteShader(tesEvalShader);
	glDeleteShader(geomShader);

	return program;
}

class GlWindow : public sb6::application
{
public:
	void startup()
	{
		_renderingProgram = CompileShaders();
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		
		//glPointSize(40.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	void render(double currentTime)
	{
		//const GLfloat bckColor[] = {sin(currentTime) * 0.5f + 0.5f, cos(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f};
		const GLfloat bckColor[] = {0.0f, 0.0f, 0.0f, 1.0f};

		glClearBufferfv(GL_COLOR, 0, bckColor);
		glUseProgram(_renderingProgram);
		//GLfloat offset[] = { sin(currentTime) * 0.5f, cos(currentTime) * 0.5f, 0.0f, 0.0f };
		GLfloat offset[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	
		//GLfloat color[] = { cos(currentTime) * 0.5f + 0.5f, sin(currentTime) * 0.5f + 0.5f, 0.0f, 1.0f };
		GLfloat color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glVertexAttrib4fv(0, offset);
		glVertexAttrib4fv(1, color);
		
		glDrawArrays(GL_PATCHES, 0, 3);
		//glUseProgram(0);
	}
	void shutdown()
	{
		glDeleteProgram(_renderingProgram);
		glDeleteVertexArrays(1, &_vao);
	}
private:
	GLuint _renderingProgram;
	GLuint _vao;
};

DECLARE_MAIN(GlWindow);

