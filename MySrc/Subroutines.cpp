//
// Shaders subroutine example.
//

#include <sb6.h>
#include <sstream>

#include "Utils.h"
#include "vmath.h"

namespace OpenGlSB
{
class Subroutines : public sb6::application
{
public:
    void startup()
    {
        _renderingProgram = GetShaderProgram("Shaders/Subroutines/Subroutines.vert", "Shaders/Subroutines/Subroutines.frag");
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glGenBuffers(1, &_vertsBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer);
        const GLfloat verts1[] =
        {
            -0.5f, -0.5f, 0.5f, 1.0f,
            -0.5f, 0.5f, 0.5f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts1), verts1, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        const GLubyte indices[] =
        {
            0, 1, 2,
            0, 2, 3
        };
        glGenBuffers(1, &_elementsBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementsBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glUseProgram(_renderingProgram);
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void render(double currentTime)
    {
        const GLfloat bckColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
        const GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, bckColor);
        glClearBufferfv(GL_DEPTH, 0, white);
        //		glClear(GL_DEPTH_BUFFER_BIT);
        GLuint sbNum[] = {0, 1};
        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &sbNum[int(currentTime) % 2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
    }

    void shutdown()
    {
        glDeleteProgram(_renderingProgram);

        glDisableVertexAttribArray(0);

        glBindVertexArray(0);
        glDeleteVertexArrays(1, &_vao);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &_vertsBuffer);
        glDeleteBuffers(1, &_elementsBuffer);
    }

private:
    GLuint _renderingProgram;
    GLuint _vao;
    GLuint _vertsBuffer;
    GLuint _elementsBuffer;

    vmath::mat4 projMatrix;
    GLfloat mvLocation = 2;
    GLfloat projLocation = 3;
};
}
