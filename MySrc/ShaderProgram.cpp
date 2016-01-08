#include "ShaderProgram.h"

ShaderProgram::ShaderProgram() : _isLinked(false), _handler(0)
{
	CreateProgram();
}

ShaderProgram::~ShaderProgram()
{
	if (_handler != 0)
		glDeleteProgram(_handler);
	ClearShaders();
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

void ShaderProgram::AttachShader(std::string filename) //todo: yep, return bool and check every time. i'll do it later
{
	if (_handler == 0)
	{
		OutputDebugStringA("Trying to attach shader. Shader program is not created!");
		return;
	}

	std::string fileExt = GetFileExtension(filename);
	auto it = ShaderTypesMap.find(fileExt);
	if (it == ShaderTypesMap.end())
	{
		OutputDebugStringA("Shader extension is not found");
		return;
	}
	GLuint shaderType = it->second;
	GLuint shader = glCreateShader(shaderType);
	
	char* shaderString = nullptr;
	GetShaderString(filename, shaderString);
	glShaderSource(shader, 1, &shaderString, nullptr);
	glCompileShader(shader);
	if (shaderString != nullptr)
	{
		delete[] shaderString;
		shaderString = nullptr;
	}

	bool compileSucessfull = CheckShader(shader);
	if (compileSucessfull)
	{
		glAttachShader(_handler, shader);
		_attachedShaders.push_back(shader);
	}
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
	if (!_isLinked)
	{
		OutputDebugStringA("Can't use shader program. It's not linked");
		return;
	}

	glUseProgram(_handler);
}

void ShaderProgram::Link()
{
	glLinkProgram(_handler);
	bool linkSucessfull = CheckShaderProgram(_handler);
	if (linkSucessfull)
	{
		_isLinked = true;
		ClearShaders();
	}
}

void ShaderProgram::ClearShaders()
{
	for (auto it = _attachedShaders.begin(); it != _attachedShaders.end(); ++it)
		glDeleteProgram(*it);
	_attachedShaders.clear();
}

void ShaderProgram::GetShaderString(std::string filename, char* resultString)
{
	FILE* fp = fopen(filename.c_str(), "rb");
	fseek(fp, 0, SEEK_END);
	size_t fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	resultString = new char[fileSize + 1];
	fread(resultString, 1, fileSize, fp);
	resultString[fileSize] = 0;
	fclose(fp);
}

bool ShaderProgram::CheckShader(GLuint shader)
{
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		OutputDebugStringA("---------------\n");
		OutputDebugStringA("Vertex Shader Error:\n");
		GLint len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		char* log = new char[len + 1];

		GLsizei byteWritten;
		glGetShaderInfoLog(shader, len, &byteWritten, log);
		log[len] = '\0';
		OutputDebugStringA(log);
		OutputDebugStringA("---------------\n");
		delete[] log;
		return false;	
	}
	return true;
}

bool ShaderProgram::CheckShaderProgram(GLuint shaderProgram)
{
	GLint status;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint len;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &len);
		char* log = new char[len + 1];
		GLsizei bytesWritten;
		glGetProgramInfoLog(shaderProgram, len, &bytesWritten, log);
		log[len] = '\0';

		OutputDebugStringA("-------------------\nShader program linking error:\n");
		OutputDebugStringA(log);
		OutputDebugStringA("--------------\n");
		delete[] log;
		return false;
	}
	return true;
}

std::string ShaderProgram::GetFileExtension(std::string filename)
{
	return filename.substr(filename.find_last_of("."));
}
