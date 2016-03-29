#pragma once
#include <string>
#include <map>
#include "sb6.h"
#include <vector>
#include <sstream>

static const std::map<std::string, GLint> ShaderTypesMap
{
    {".vert", GL_VERTEX_SHADER},
    {".frag", GL_FRAGMENT_SHADER},
    {".geom", GL_GEOMETRY_SHADER},
    {".tesc", GL_TESS_CONTROL_SHADER},
    {".tese", GL_TESS_EVALUATION_SHADER},
    {".comp", GL_COMPUTE_SHADER}
};

static const std::map<GLint, std::string> ShaderTypesSringMap
{
    {GL_VERTEX_SHADER, "Vertex Shader"},
    {GL_FRAGMENT_SHADER, "Fragment Shader"},
    {GL_GEOMETRY_SHADER, "Geometry Shader"},
    {GL_TESS_CONTROL_SHADER, "Tesselation Control Shader"},
    {GL_TESS_EVALUATION_SHADER, "Tesselation Evaluation Shader"},
    {GL_COMPUTE_SHADER, "Compute Shader"}
};

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();
    void CreateProgram();
    void CreateAndLinkProgram(std::string vertFilename, std::string fragFilename);
    void DeleteProgram();
    void AttachShader(std::string filename);
    GLuint GetHandler();
    bool IsLinked();
    void Use();
    void Link();
    void ClearShaders();
    void SetUniform(float val, std::string name);
    void SetUniform(float val, int index);
    void SetUniform(int val, std::string name);
    void SetUniform(int val, int index);
    //todo blabla other uniform and vertex attribs
private:
    void AddSourceToShader(std::string filename, GLuint shader);
    bool CheckShader(GLuint shader, GLint shaderType, std::string filename);
    bool CheckShaderProgram(GLuint shaderProgram);
    bool _isLinked;
    GLuint _handler;
    std::string GetFileExtension(std::string filename);
    std::vector<GLuint> _attachedShaders;
};

