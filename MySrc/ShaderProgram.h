#pragma once
#include <string>
#include <map>
#include "sb6.h"
#include <vector>

static const std::map<std::string, GLint> ShaderTypesMap
{
	{ ".vert", GL_VERTEX_SHADER },
	{ ".frag", GL_FRAGMENT_SHADER },
	{ ".geom", GL_GEOMETRY_SHADER },
	{ ".tesc", GL_TESS_CONTROL_SHADER },
	{ ".tese", GL_TESS_EVALUATION_SHADER }
};

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();
	void CreateProgram();
	void DeleteProgram();
	void AttachShader(std::string filename);
	GLuint GetHandler();
	bool IsLinked();
	void Use();
	void Link();
	void ClearShaders();
private:
	void AddSourceToShader(std::string filename, GLuint shader);
	bool CheckShader(GLuint shader);
	bool CheckShaderProgram(GLuint shaderProgram);
	bool _isLinked;
	GLuint _handler;
	std::string GetFileExtension(std::string filename);
	std::vector<GLuint> _attachedShaders;
};
