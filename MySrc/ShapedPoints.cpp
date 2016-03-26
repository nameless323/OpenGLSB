#include <sb6.h>
#include <sb6ktx.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"
#include "ShaderProgram.h"

static unsigned int Seed = 0x13371337;

static const unsigned int NUM_STARS = 2000;

static float RandomFloat()
{
    float res;
    unsigned int tmp;

    Seed *= 16807;
    tmp = Seed ^ (Seed >> 4) ^ (Seed << 15);
    *((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;
    return (res - 1.0f);
}

class ShapedPoints : public sb6::application
{
public:
    void startup()
    {
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
        _renderingProgram.CreateProgram();
        _renderingProgram.AttachShader("Shaders/ShapedPoints/ShapedPoints.vert");
        _renderingProgram.AttachShader("Shaders/ShapedPoints/ShapedPoints.frag");
        _renderingProgram.Link();
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

       
        _renderingProgram.Use();
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void render(double currentTime)
    {
        const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, bckColor);
        glClearBufferfv(GL_DEPTH, 0, white);

        glPointSize(200.0f);
        glBindVertexArray(_vao);
        glDrawArrays(GL_POINTS, 0, 4);
    }

    void shutdown()
    {
        _renderingProgram.DeleteProgram();

        glDisableVertexAttribArray(0);

        glBindVertexArray(0);
        glDeleteVertexArrays(1, &_vao);

    }

private:
    ShaderProgram _renderingProgram;
    GLuint _vao;
    GLuint _starBuffer;
    GLuint _elementsBuffer;
    GLuint _starTexture;

    vmath::mat4 projMatrix;
    GLfloat mvLocation = 2;
    GLfloat projLocation = 3;

    struct
    {
        float Time;
        float Proj;
    } Uniforms;
};

DECLARE_MAIN(ShapedPoints);

