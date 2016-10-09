//
// Tesselation of a cube.
//

#include <sb6.h>

#include "vmath.h"
#include "../ShaderProgram.h"

namespace OpenGlSB
{
using vmath::mat4;
using vmath::translate;
using vmath::rotate;
using vmath::perspective;

//#define MANY_CUBES
class TessCube : public sb6::application
{
public:
    void startup()
    {
        rotationMat = translate(0.0f, 0.0f, 0.0f);
        _wireframeMode = true;
        _multicube = false;
        _rotate = true;
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
        _shader.CreateProgram();
        _shader.AttachShader("Shaders/Tesselation/Cube/TessCube.vert");
        _shader.AttachShader("Shaders/Tesselation/Cube/TessCube.frag");
        _shader.AttachShader("Shaders/Tesselation/Cube/TessCube.tesc");
        _shader.AttachShader("Shaders/Tesselation/Cube/TessCube.tese");
        _shader.Link();

        _mvLocation = glGetUniformLocation(_shader.GetHandler(), "mv_matrix");
        _projLocation = glGetUniformLocation(_shader.GetHandler(), "proj_matrix");

        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);

        glGenBuffers(1, &_vertsBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer);
        const GLfloat verts[] =
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        const GLushort indices[] =
        {
            0, 1, 2, 3,
            2, 3, 4, 5,
            4, 5, 6, 7,
            6, 7, 0, 1,
            0, 2, 6, 4,
            1, 7, 3, 5
        };
        glGenBuffers(1, &_elementsBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementsBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        _shader.Use();
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void render(double currentTime)
    {
        _shader.Use();
        int i;
        const GLfloat bckColor[] = {0.0f, 0.25f, 0.0f, 1.0f};
        const GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glViewport(0, 0, info.windowWidth, info.windowHeight);
        glClearBufferfv(GL_COLOR, 0, bckColor);
        glClearBufferfv(GL_DEPTH, 0, white);

        _shader.Use();

        _projMatrix = perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);
        glUniformMatrix4fv(_projLocation, 1, GL_FALSE, _projMatrix);
        glPatchParameteri(GL_PATCH_VERTICES, 4);

        glPolygonMode(GL_FRONT_AND_BACK, _wireframeMode ? GL_LINE : GL_FILL);

        if (_multicube)
        {
            for (i = 0; i < 100; i++)
            {
                float f = (float)i + (float)currentTime * 0.03;

                mat4 mv = translate(0.0f, 0.0f, -10.0f) *
                        translate(sinf(2.1f * f) * 4.0f, cosf(1.7f * f) * 4.0f,
                                  sinf(4.3f * f) * cosf(3.5f * f) * 30.0f) *
                        rotate((float)currentTime * 3.0f, 1.0f, 0.0f, 0.0f) *
                        rotate((float)currentTime * 5.0f, 0.0f, 1.0f, 0.0f);
                glUniformMatrix4fv(_mvLocation, 1, GL_FALSE, mv);
                glDrawElements(GL_PATCHES, 24, GL_UNSIGNED_SHORT, nullptr);
            }
        }
        else
        {
            float f = (float)currentTime * 0.3f;

            if (_rotate)
            {
                rotationMat = rotate((float)currentTime * 81.0f, 0.0f, 1.0f, 0.0f) *
                        rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);
            }
            mat4 mv = translate(0.0f, 0.0f, -0.5f) * rotationMat;
            glUniformMatrix4fv(_mvLocation, 1, GL_FALSE, mv);
            glDrawElements(GL_PATCHES, 24, GL_UNSIGNED_SHORT, nullptr);
        }
    }

    void shutdown()
    {
        _shader.DeleteProgram();

        glDisableVertexAttribArray(0);

        glBindVertexArray(0);
        glDeleteVertexArrays(1, &_vao);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &_vertsBuffer);
        glDeleteBuffers(1, &_elementsBuffer);
    }

    virtual void onKey(int key, int action)
    {
        if (action == 1)
        {
            switch (key)
            {
                case 'W': _wireframeMode = !_wireframeMode;
                    break;
                case 'M': _multicube = !_multicube;
                    break;
                case 'R': _rotate = !_rotate;
                    break;
            }
        }
    }

private:
    ShaderProgram _shader;
    GLuint _vao;
    GLuint _vertsBuffer;
    GLuint _elementsBuffer;

    vmath::mat4 _projMatrix;
    GLfloat _mvLocation = 2;
    GLfloat _projLocation = 3;
    bool _wireframeMode;
    bool _multicube;
    bool _rotate;
    mat4 rotationMat;
};
}
