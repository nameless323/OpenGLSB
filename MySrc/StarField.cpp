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

class StarField : public sb6::application
{
public:
    void startup()
    {
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
        _renderingProgram.CreateProgram();
        _renderingProgram.AttachShader("Shaders/Starfield/Starfield.vert");
        _renderingProgram.AttachShader("Shaders/Starfield/Starfield.frag");
        _renderingProgram.Link();
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        Uniforms.Time = glGetUniformLocation(_renderingProgram.GetHandler(), "time");
        Uniforms.Proj = glGetUniformLocation(_renderingProgram.GetHandler(), "proj_matrix");

        _starTexture = sb6::ktx::file::load("media/textures/star.ktx");

        struct starT
        {
            vmath::vec3 Position;
            vmath::vec3 Color;
        };

        glGenBuffers(1, &_starBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _starBuffer);
        glBufferData(GL_ARRAY_BUFFER, NUM_STARS * sizeof(starT), nullptr, GL_STATIC_DRAW);

        starT* star = (starT*)glMapBufferRange(GL_ARRAY_BUFFER, 0, NUM_STARS * sizeof(starT), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        for (int i = 0; i < 1000; i++)
        {
            star[i].Position[0] = (RandomFloat() * 2.0f - 1.0f) * 100.0f;
            star[i].Position[1] = (RandomFloat() * 2.0f - 1.0f) * 100.0f;
            star[i].Position[2] = RandomFloat();
            star[i].Color[0] = 0.8f + RandomFloat() * 0.2f;
            star[i].Color[1] = 0.8f + RandomFloat() * 0.2f;
            star[i].Color[2] = 0.8f + RandomFloat() * 0.2f;
        }

        glUnmapBuffer(GL_ARRAY_BUFFER);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(starT), nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(starT), (void*)sizeof(vmath::vec3));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);


        _renderingProgram.Use();
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

        float t = (float)currentTime;
        vmath::mat4 proj = vmath::perspective(50.0f,
                                              (float)info.windowWidth / (float)info.windowHeight,
                                              0.1f,
                                              1000.0f);
        t *= 0.3f;
//        t -= floor(t);
        glViewport(0, 0, info.windowWidth, info.windowHeight);
        
        glUniform1f(Uniforms.Time, t);
        glUniformMatrix4fv(Uniforms.Proj, 1, GL_FALSE, proj);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        glBindVertexArray(_vao);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glDrawArrays(GL_POINTS, 0, NUM_STARS);
    }

    void shutdown()
    {
        _renderingProgram.DeleteProgram();

        glDisableVertexAttribArray(0);

        glBindVertexArray(0);
        glDeleteVertexArrays(1, &_vao);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &_starBuffer);
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

DECLARE_MAIN(StarField);

