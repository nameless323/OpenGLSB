#include <sb6.h>
#include <object.h>

#include "vmath.h"
#include "ShaderProgram.h"

namespace OpenGlSB
{
using vmath::mat4;

class RenderToTex : public sb6::application
{
public:
    void startup()
    {
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
        _shader1.CreateProgram();
        _shader1.AttachShader("Shaders/RenderToTex/RenderToTex.vert");
        _shader1.AttachShader("Shaders/RenderToTex/RenderToTex1.frag");
        _shader1.Link();

        _shader2.CreateProgram();
        _shader2.AttachShader("Shaders/RenderToTex/RenderToTex.vert");
        _shader2.AttachShader("Shaders/RenderToTex/RenderToTex2.frag");
        _shader2.Link();

        _mvLocation = glGetUniformLocation(_shader1.GetHandler(), "mv_matrix");
        _projLocation = glGetUniformLocation(_shader1.GetHandler(), "proj_matrix");

        _mvLocation2 = glGetUniformLocation(_shader2.GetHandler(), "mv_matrix");
        _projLocation2 = glGetUniformLocation(_shader2.GetHandler(), "proj_matrix");


        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

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

        //pos and uv
        static const GLfloat vertData[] =
        {
            -0.25f, -0.25f, 0.25f, 0.0f, 1.0f,
            -0.25f, -0.25f, -0.25f, 0.0f, 0.0f,
            0.25f, -0.25f, -0.25f, 1.0f, 0.0f,

            0.25f, -0.25f, -0.25f, 1.0f, 0.0f,
            0.25f, -0.25f, 0.25f, 1.0f, 1.0f,
            -0.25f, -0.25f, 0.25f, 0.0f, 1.0f,

            0.25f, -0.25f, -0.25f, 0.0f, 0.0f,
            0.25f, 0.25f, -0.25f, 1.0f, 0.0f,
            0.25f, -0.25f, 0.25f, 0.0f, 1.0f,

            0.25f, 0.25f, -0.25f, 1.0f, 0.0f,
            0.25f, 0.25f, 0.25f, 1.0f, 1.0f,
            0.25f, -0.25f, 0.25f, 0.0f, 1.0f,

            0.25f, 0.25f, -0.25f, 1.0f, 0.0f,
            -0.25f, 0.25f, -0.25f, 0.0f, 0.0f,
            0.25f, 0.25f, 0.25f, 1.0f, 1.0f,

            -0.25f, 0.25f, -0.25f, 0.0f, 0.0f,
            -0.25f, 0.25f, 0.25f, 0.0f, 1.0f,
            0.25f, 0.25f, 0.25f, 1.0f, 1.0f,

            -0.25f, 0.25f, -0.25f, 1.0f, 0.0f,
            -0.25f, -0.25f, -0.25f, 0.0f, 0.0f,
            -0.25f, 0.25f, 0.25f, 1.0f, 1.0f,

            -0.25f, -0.25f, -0.25f, 0.0f, 0.0f,
            -0.25f, -0.25f, 0.25f, 0.0f, 1.0f,
            -0.25f, 0.25f, 0.25f, 1.0f, 1.0f,

            -0.25f, 0.25f, -0.25f, 0.0f, 1.0f,
            0.25f, 0.25f, -0.25f, 1.0f, 1.0f,
            0.25f, -0.25f, -0.25f, 1.0f, 0.0f,

            0.25f, -0.25f, -0.25f, 1.0f, 0.0f,
            -0.25f, -0.25f, -0.25f, 0.0f, 0.0f,
            -0.25f, 0.25f, -0.25f, 0.0f, 1.0f,

            -0.25f, -0.25f, 0.25f, 0.0f, 0.0f,
            0.25f, -0.25f, 0.25f, 1.0f, 0.0f,
            0.25f, 0.25f, 0.25f, 1.0f, 1.0f,

            0.25f, 0.25f, 0.25f, 1.0f, 1.0f,
            -0.25f, 0.25f, 0.25f, 0.0f, 1.0f,
            -0.25f, -0.25f, 0.25f, 0.0f, 0.0f,
        };

        glGenBuffers(1, &_vertsBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertData), vertData, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &_elementsBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementsBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glGenFramebuffers(1, &_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

        glGenTextures(1, &_colorTex);
        glBindTexture(GL_TEXTURE_2D, _colorTex);
        glTexStorage2D(GL_TEXTURE_2D, 9, GL_RGBA8, 512, 512);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _colorTex, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorTex, 0);

        glGenTextures(1, &_depthTex);
        glBindTexture(GL_TEXTURE_2D, _depthTex);
        glTexStorage2D(GL_TEXTURE_2D, 9, GL_DEPTH_COMPONENT32F, 512, 512);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthTex, 0);
        GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, drawBuffers);
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void render(double currentTime)
    {
        static const GLfloat green[] = {0.0f, 1.0f, 0.0f, 1.0f};
        static const GLfloat blue[] = {0.0f, 0.0f, 1.0f, 1.0f};
        static const GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};

        mat4 proj = vmath::perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);

        float f = (float)currentTime * 0.3f;
        mat4 mv = vmath::translate(0.0f, 0.0f, -2.0f) *
                vmath::translate(sinf(2.1f * f) * 0.5f, cosf(1.7f * f) * 0.5f, sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
                vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
                vmath::rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        glViewport(0, 0, 512, 512);

        glClearBufferfv(GL_COLOR, 0, green);
        glClearBufferfv(GL_DEPTH, 0, white);
        _shader1.Use();

        glUniformMatrix4fv(_projLocation, 1, GL_FALSE, proj);
        glUniformMatrix4fv(_mvLocation, 1, GL_FALSE, mv);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, info.windowWidth, info.windowHeight);
        glClearBufferfv(GL_COLOR, 0, blue);
        glClearBufferfv(GL_DEPTH, 0, white);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _colorTex);
        _shader2.Use();
        glUniformMatrix4fv(_projLocation2, 1, GL_FALSE, proj);
        glUniformMatrix4fv(_mvLocation2, 1, GL_FALSE, mv);

        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void shutdown()
    {
        _shader1.DeleteProgram();
        _shader2.DeleteProgram();
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &_vao);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDeleteBuffers(1, &_vertsBuffer);
        glDeleteBuffers(1, &_elementsBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &_fbo);
        glDeleteTextures(1, &_colorTex);
        glDeleteTextures(1, &_depthTex);
    }

private:
    ShaderProgram _shader1;
    ShaderProgram _shader2;
    GLuint _vao;
    GLuint _vertsBuffer;
    GLuint _elementsBuffer;
    GLuint _fbo;
    GLuint _colorTex;
    GLuint _depthTex;

    mat4 _projMatrix;
    GLint _mvLocation = 2;
    GLint _projLocation;
    GLint _mvLocation2 = 2;
    GLint _projLocation2 = 3;
    GLint _explodeFactor;
    GLint _normalLengthLocation;
    GLint _viewpointLocation;

    sb6::object _object;
};
}
