//
// Geometry shader based tesselation.
//

#include <sb6.h>

#include <object.h>

#include "vmath.h"
#include "../ShaderProgram.h"

namespace OpenGlSB
{
using vmath::mat4;

class GSTesselation : public sb6::application
{
public:
    void startup()
    {
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
        _shader.CreateProgram();
        _shader.AttachShader("Shaders/GeomShaders/Tesselation/GSTesselation.vert");
        _shader.AttachShader("Shaders/GeomShaders/Tesselation/GSTesselation.geom");
        _shader.AttachShader("Shaders/GeomShaders/Tesselation/GSTesselation.frag");
        _shader.Link();

        _mvLocation = glGetUniformLocation(_shader.GetHandler(), "mvMatrix");
        _mvpLocation = glGetUniformLocation(_shader.GetHandler(), "mvpMatrix");
        _stretchLocation = glGetUniformLocation(_shader.GetHandler(), "stretch");

        static const GLfloat tetrahedronVerts[] =
        {
            0.000f, 0.000f, 1.000f,
            0.943f, 0.000f, -0.333f,
            -0.471f, 0.816f, -0.333f,
            -0.471f, -0.816f, -0.333f
        };

        static const GLushort tetrahedronIndices[] =
        {
            0, 1, 2,
            0, 2, 3,
            0, 3, 1,
            3, 2, 1
        };
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        glGenBuffers(1, &_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tetrahedronVerts) + sizeof(tetrahedronIndices), nullptr, GL_STATIC_DRAW);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(tetrahedronIndices), tetrahedronIndices);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tetrahedronIndices), sizeof(tetrahedronVerts), tetrahedronVerts);

        glBindBuffer(GL_ARRAY_BUFFER, _buffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)sizeof(tetrahedronIndices));
        glEnableVertexAttribArray(0);
        glEnable(GL_CULL_FACE);
        //glDisable(GL_CULL_FACE);
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
        const GLfloat bckColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
        const GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, bckColor);
        glClearBufferfv(GL_DEPTH, 0, white);
        _shader.Use();

        mat4 projMatrix = vmath::perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);
        mat4 mvMatrix = vmath::translate(0.0f, 0.0f, -10.5f) *
                vmath::rotate((float)currentTime * 71.0f, 0.0f, 1.0f, 0.0f) *
                vmath::rotate((float)currentTime * 10.0f, 1.0f, 0.0f, 0.0f);

        glUniformMatrix4fv(_mvpLocation, 1, GL_FALSE, projMatrix * mvMatrix);
        glUniformMatrix4fv(_mvLocation, 1, GL_FALSE, mvMatrix);

        glUniform1f(_stretchLocation, sinf((float)currentTime * 4.0) * 0.75f + 0.1f);
        glBindVertexArray(_vao);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_SHORT, nullptr);
    }

    void shutdown()
    {
        _shader.DeleteProgram();
        glDeleteVertexArrays(1, &_vao);
        glDeleteBuffers(1, &_buffer);
    }

private:
    ShaderProgram _shader;
    GLuint _vao;
    GLuint _buffer;

    mat4 _projMatrix;
    GLint _mvLocation = 2;
    GLint _mvpLocation = 3;
    GLint _projLocation;
    GLint _stretchLocation;
    GLint _viewpointLocation;

    sb6::object _object;
};
}
