#include "ShaderProgram.h"

ShaderProgram::ShaderProgram() : _isLinked(false), _handler(0)
{
	CreateProgram();
	GetFileExtension("blabalbal.v.frag");
}

void ShaderProgram::CreateProgram()
{
	_handler = glCreateProgram();
}

void ShaderProgram::DeleteProgram()
{
	_isLinked = false;
	if (_handler != 0)
		glDeleteProgram(_handler);
	_handler = 0;
}

void ShaderProgram::AttachShader(std::string filename)
{
}

GLuint ShaderProgram::GetHandler()
{
	return _handler;
}

bool ShaderProgram::IsLinked()
{
	return _isLinked;
}

void ShaderProgram::Use()
{
	if (!_isLinked) return;
	glUseProgram(_handler);
}

void ShaderProgram::Link()
{
	glLinkProgram(_handler);
	_isLinked = true;
}

std::string ShaderProgram::GetFileExtension(std::string filename)
{
	return filename.substr(filename.find_last_of("."));
}
