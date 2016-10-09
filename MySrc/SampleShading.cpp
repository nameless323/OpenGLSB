//
// GL multisampling demonstration.
//

#include <sb6.h>
#include <object.h>

#include "vmath.h"
#include "ShaderProgram.h"

namespace OpenGlSB
{
using vmath::mat4;

class SampleShading : public sb6::application
{
public:
    SampleShading() : _manyCubes(false), _wireframe(false), _paused(false), _sampleShading(false)
    {
    }


    void init() override
    {
        sb6::application::init();
        info.samples = 8;
    }

    void DrawManyCubes(float currentTime)
    {
        for (int i = 0; i < 24; i++)
        {
            float f = (float)i + (float)currentTime * 0.3f;
            mat4 mv = vmath::translate(0.0f, 0.0f, -20.0f) *
                    vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
                    vmath::rotate((float)currentTime * 21.0f, 1.0f, 0.0f, 0.0f) *
                    vmath::translate(sinf(2.1f * f) * 2.0f,
                                     cosf(1.7f * f) * 2.0f,
                                     sinf(1.3f * f) * cosf(1.5f * f) * 2.0f);
            glUniformMatrix4fv(_mvLocation, 1, GL_FALSE, mv);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
        }
    }

    void DrawSingleCube(float currentTime)
    {
        float f = (float)currentTime * 0.1f;
        mat4 mv = vmath::translate(0.0f, 0.0f, -2.0f) *
                vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
                vmath::rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);
        glUniformMatrix4fv(_mvLocation, 1, GL_FALSE, mv);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
    }

    void startup()
    {
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
        _shader.CreateProgram();
        _shader.AttachShader("Shaders/SampleShading/SampleShading.vert");
        _shader.AttachShader("Shaders/SampleShading/SampleShading.frag");
        _shader.Link();
        _mvLocation = glGetUniformLocation(_shader.GetHandler(), "mv_matrix");
        _projLocation = glGetUniformLocation(_shader.GetHandler(), "proj_matrix");

        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        static const GLushort vertIndices[] =
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
        static const GLfloat vertPositions[] =
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

        glGenBuffers(1, &_positionBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _positionBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertPositions), vertPositions, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &_elementsBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementsBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertIndices), vertIndices, GL_STATIC_DRAW);
        glEnable(GL_CULL_FACE);
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void render(double currentTime)
    {
        static const GLfloat black[] = {0.0f, 0.0f, 0.0f, 1.0f};
        static const GLfloat gray[] = {0.1f, 0.1f, 0.1f, 1.0f};
        static const GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, black);

        if (_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        _shader.Use();

        mat4 proj = vmath::perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);
        glUniformMatrix4fv(_projLocation, 1, GL_FALSE, proj);

        static double ct = 0;
        static double prevT = 0;
        double dt = 0;

        if (!_paused)
            dt = prevT - currentTime;
        prevT = currentTime;
        ct += dt;

        if (_sampleShading)
        {
            glEnable(GL_SAMPLE_SHADING);
            glMinSampleShading(1.0f);
        }
        else
        {
            glDisable(GL_SAMPLE_SHADING);
        }
        if (_manyCubes)
        {
            DrawManyCubes(ct);
        }
        else
        {
            DrawSingleCube(ct);
        }
    }

    void shutdown()
    {
        _shader.DeleteProgram();
        glBindVertexArray(0);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDeleteVertexArrays(1, &_vao);
        glDeleteBuffers(1, &_elementsBuffer);
        glDeleteBuffers(1, &_positionBuffer);
    }

    void onKey(int key, int action) override
    {
        if (action)
        {
            if (key == 'P')
                _manyCubes = !_manyCubes;
            if (key == 'Q')
            {
                glEnable(GL_MULTISAMPLE);
            }
            if (key == 'W')
            {
                glDisable(GL_MULTISAMPLE);
            }
            if (key == 'E')
            {
                _sampleShading = !_sampleShading;
            }
            if (key == 'R')
            {
                glDisable(GL_BLEND);
                glDisable(GL_POLYGON_SMOOTH);
            }
            if (key == 'A')
                _wireframe = !_wireframe;
            if (key == 'S')
                _paused = !_paused;
        }
    }

private:
    ShaderProgram _shader;
    GLuint _vao;
    GLuint _elementsBuffer;
    GLuint _positionBuffer;

    mat4 _projMatrix;
    GLint _mvLocation = 2;
    GLint _projLocation;
    GLint _mvLocation2 = 2;
    GLint _projLocation2 = 3;
    GLint _explodeFactor;
    GLint _normalLengthLocation;
    GLint _viewpointLocation;
    bool _manyCubes;
    bool _wireframe;
    bool _paused;
    bool _sampleShading;

    sb6::object _object;
};
}
