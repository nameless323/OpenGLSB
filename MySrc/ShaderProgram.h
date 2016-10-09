//
// Shader program helper class.
//

#pragma once

#include <string>
#include <map>
#include <vector>

#include "sb6.h"

namespace OpenGlSB
{
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
    /**
     * \brief Create shader program.
     */
    void CreateProgram();
    /**
     * \brief Create and link program.
     * \param vertFilename Filename for vertex shader.
     * \param fragFilename Filename for fragment shader.
     */
    void CreateAndLinkProgram(std::string vertFilename, std::string fragFilename);
    /**
     * \brief Delete shader.
     */
    void DeleteProgram();
    /**
     * \brief Attach shader to program.
     */
    void AttachShader(std::string filename);
    /**
     * \brief Get shader program handler.
     */
    GLuint GetHandler();
    /**
     * \brief Check if shader is linked.
     */
    bool IsLinked();
    /**
     * \brief Use shader program.
     */
    void Use();
    /**
     * \brief Link shader program.
     */
    void Link();
    /**
     * \brief Delete all shaders from program.
     */
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
}
