//
// Grass field using instancing technique.
//

#include <sb6.h>
#include <cmath>
#include <sstream>
#include <sb6ktx.h>

#include "Utils.h"
#include "vmath.h"

namespace OpenGlSB
{
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
        glEnableVertexAttribArray(0);

        _shaderProgram = GetShaderProgram("Shaders/GrassInstancing/GrassInstancing.vert", "Shaders/GrassInstancing/GrassInstancing.frag");

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

    void render(double dt) override
    {
        float t = (float)dt * 0.02f;
        float r = 550.0f;

        static const GLfloat black[] = {0.0f, 0.0f, 0.0f, 1.0f};
        static const GLfloat one = 1.0f;
        glClearBufferfv(GL_COLOR, 0, black);
        glClearBufferfv(GL_DEPTH, 0, &one);

        vmath::mat4 mv = vmath::lookat(vmath::vec3(sinf(t) * r, 25.0f, cosf(t) * r),
                                       vmath::vec3(0.0f, -50.0f, 0.0f),
                                       vmath::vec3(0.0f, 1.0f, 0.0f));
        vmath::mat4 proj = vmath::perspective(45.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);

        glUseProgram(_shaderProgram);
        glUniformMatrix4fv(uniforms.mvpMatrix, 1, GL_FALSE, proj * mv);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glViewport(0, 0, info.windowWidth, info.windowHeight);

        glBindVertexArray(_vao);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, 1024 * 1024);
    }

    void shutdown(void)
    {
        glDeleteProgram(_shaderProgram);
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
}
