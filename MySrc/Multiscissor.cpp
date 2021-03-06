//
// Using multiple scissors rect example.
//

#include <sb6.h>
#include <object.h>

#include "vmath.h"
#include "ShaderProgram.h"

namespace OpenGlSB
{
using vmath::mat4;

class Multiscissor : public sb6::application
{
public:
    void startup()
    {
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
        _shader.CreateProgram();
        _shader.AttachShader("Shaders/Multiscissor/Multiscissor.vert");
        _shader.AttachShader("Shaders/Multiscissor/Multiscissor.geom");
        _shader.AttachShader("Shaders/Multiscissor/Multiscissor.frag");
        _shader.Link();

        _mvLocation = glGetUniformLocation(_shader.GetHandler(), "mv_matrix");
        _projLocation = glGetUniformLocation(_shader.GetHandler(), "proj_matrix");

        static const GLfloat vertices[] =
        {
            -0.25f, -0.25f, -0.25f,
            -0.25f, 0.25f, -0.25f,
            0.25f, -0.25f, -0.25f,
            0.25f, 0.25f, -0.25f,
            0.25f, -0.25f, 0.25f,
            0.25f, 0.25f, 0.25f,
            -0.25f, -0.25f, 0.25f,
            -0.25f, 0.25f, 0.25f,
        };

        static const GLushort indices[] =
        {
            0, 1, 2,
            2, 1, 3,
            2, 3, 4,
            4, 3, 5,
            4, 5, 6,
            6, 5, 7,
            6, 7, 0,
            0, 7, 1,
            6, 0, 2,
            2, 4, 6,
            7, 5, 3,
            7, 3, 1
        };

        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        glGenBuffers(1, &_vertsBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &_elementsBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementsBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glGenBuffers(1, &_uniformBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER, _uniformBuffer);
        glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(mat4), nullptr, GL_DYNAMIC_DRAW);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        _shader.Use();
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void render(double currentTime)
    {
        glDisable(GL_SCISSOR_TEST);
        glViewport(0, 0, info.windowWidth, info.windowHeight);

        static const GLfloat bckColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
        static const GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, bckColor);
        glClearBufferfv(GL_DEPTH, 0, white);
        _shader.Use();
        glEnable(GL_SCISSOR_TEST);


        float scissorWidth = (float)(7 * info.windowWidth) / 16.0f;
        float scissorHeight = (float)(7 * info.windowHeight) / 16.0f;
        glScissorIndexed(0, 0, 0, scissorWidth, scissorHeight);
        glScissorIndexed(1, info.windowWidth - scissorWidth, 0, scissorWidth, scissorHeight);
        glScissorIndexed(2, 0, info.windowHeight - scissorHeight, scissorWidth, scissorHeight);
        glScissorIndexed(3, info.windowWidth - scissorWidth, info.windowHeight - scissorHeight, scissorWidth, scissorHeight);

        mat4 projMat = vmath::perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);
        float f = (float)currentTime * 0.3f;

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _uniformBuffer);
        mat4* mvArray = (mat4*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 4 * sizeof(mat4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        for (int i = 0; i < 4; i++)
        {
            mvArray[i] = projMat * vmath::translate(0.0f, 0.0f, -2.0f) *
                    vmath::rotate((float)currentTime * 45.0f * (float)(i + 1), 0.0f, 1.0f, 0.0f) *
                    vmath::rotate((float)currentTime * 81.0f * (float)(i + 1), 1.0f, 0.0f, 0.0f);
        }
        glUnmapBuffer(GL_UNIFORM_BUFFER);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
    }

    void shutdown()
    {
        _shader.DeleteProgram();
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &_vao);
        glDeleteBuffers(1, &_elementsBuffer);
        glDeleteBuffers(1, &_vertsBuffer);
        glDeleteBuffers(1, &_elementsBuffer);
    }

private:
    ShaderProgram _shader;
    GLuint _vao;
    GLuint _vertsBuffer;
    GLuint _elementsBuffer;
    GLuint _uniformBuffer;

    mat4 _projMatrix;
    GLint _mvLocation = 2;
    GLint _mvpLocation = 3;
    GLint _projLocation;
    GLint _normalLengthLocation;
    GLint _viewpointLocation;

    sb6::object _object;
};
}
