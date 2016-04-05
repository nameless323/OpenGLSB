#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sb6ktx.h>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"
#include "ShaderProgram.h"
#include <object.h>

enum
{
    WORKGROUP_SIZE = 256,
    NUM_WORKGROUPS = 64,
    FLOCK_SIZE = (NUM_WORKGROUPS * WORKGROUP_SIZE)
};

class Flocking : public sb6::application
{
public:
    void LoadShaders()
    {
        _shader.CreateProgram();
        _shader.AttachShader("Shaders/Flocking/Flocking.vert");
        _shader.AttachShader("Shaders/Flocking/Flocking.frag");
        _shader.Link();

        Uniforms.Render.MVP = glGetUniformLocation(_shader.GetHandler(), "mvp");

        _compShader.CreateProgram();
        _compShader.AttachShader("Shaders/Flocking/Flocking.comp");

        Uniforms.Update.Goal = glGetUniformLocation(_compShader.GetHandler(), "goal");
    }

    void startup() override
    {
        static const vmath::vec3 geom[] =
            {
                // Positions
                vmath::vec3(-5.0f, 1.0f, 0.0f),
                vmath::vec3(-1.0f, 1.5f, 0.0f),
                vmath::vec3(-1.0f, 1.5f, 7.0f),
                vmath::vec3(0.0f, 0.0f, 0.0f),
                vmath::vec3(0.0f, 0.0f, 10.0f),
                vmath::vec3(1.0f, 1.5f, 0.0f),
                vmath::vec3(1.0f, 1.5f, 7.0f),
                vmath::vec3(5.0f, 1.0f, 0.0f),

                // Normals
                vmath::vec3(0.0f),
                vmath::vec3(0.0f),
                vmath::vec3(0.107f, -0.859f, 0.00f),
                vmath::vec3(0.832f, 0.554f, 0.00f),
                vmath::vec3(-0.59f, -0.395f, 0.00f),
                vmath::vec3(-0.832f, 0.554f, 0.00f),
                vmath::vec3(0.295f, -0.196f, 0.00f),
                vmath::vec3(0.124f, 0.992f, 0.00f),
            };

        LoadShaders();

        glGenBuffers(2, _flockBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, _flockBuffer[0]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, FLOCK_SIZE * sizeof(FlockMember), nullptr, GL_DYNAMIC_COPY);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, _flockBuffer[1]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, FLOCK_SIZE * sizeof(FlockMember), nullptr, GL_DYNAMIC_COPY);

        glGenBuffers(1, &_geomBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _geomBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(geom), geom, GL_STATIC_DRAW);
        glGenVertexArrays(2, _flockRenderVao);

        for (int i = 0; i < 2; i++)
        {
            glBindVertexArray(_flockRenderVao[i]);
            glBindBuffer(GL_ARRAY_BUFFER, _geomBuffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(8 * sizeof(vmath::vec3)));

            glBindBuffer(GL_ARRAY_BUFFER, _flockBuffer[i]);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FlockMember), nullptr);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(FlockMember), (void*)sizeof(vmath::vec4));
            glVertexAttribDivisor(2, 1);
            glVertexAttribDivisor(3, 1);

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);
        }
        glBindBuffer(GL_ARRAY_BUFFER, _flockBuffer[0]);
        FlockMember* ptr = (FlockMember*)glMapBufferRange(GL_ARRAY_BUFFER, 0, FLOCK_SIZE * sizeof(FlockMember), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        for (int i = 0; i < FLOCK_SIZE; i ++)
        {
            ptr[i].Position = (vmath::vec3::random() - vmath::vec3(0.5f)) * 300.0f;
            ptr[i].Velocity = (vmath::vec3::random() - vmath::vec3(0.5f));
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void render(double currentTime) override
    {
        float t = (float)currentTime;
        static const float black[] = {0.0f, 0.0f, 0.0f, 1.0f};
        static const float one = 1.0f;

        _compShader.Use();
        vmath::vec3 goal = vmath::vec3
            (
                sinf(t * 0.34f),
                cosf(t * 0.29f),
                sinf(t * 0.12f) * cosf(t * 0.5f)
            );
        goal = goal * vmath::vec3(35.0f, 25.0f, 60.0f);

        glUniform3fv(Uniforms.Update.Goal, 1, goal);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _flockBuffer[_frameIndex]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _flockBuffer[_frameIndex ^ 1]);

        glDispatchCompute(NUM_WORKGROUPS, 1, 1);
        glViewport(0, 0, info.windowWidth, info.windowHeight);
        glClearBufferfv(GL_COLOR, 0, black);
        glClearBufferfv(GL_DEPTH, 0, &one);

        _shader.Use();
        vmath::mat4 mv = vmath::lookat(vmath::vec3(0.0f, 0.0f, -400.0f),
                                       vmath::vec3(0.0f, 0.0f, 0.0f),
                                       vmath::vec3(0.0f, 1.0f, 0.0f));
        vmath::mat4 proj = vmath::perspective(60.0f,
                                              (float)info.windowWidth / (float)info.windowHeight,
                                              0.1f,
                                              3000.0f);
        vmath::mat4 mvp = proj * mv;

        glUniformMatrix4fv(Uniforms.Render.MVP, 1, GL_FALSE, mvp);
        glBindVertexArray(_flockRenderVao[_frameIndex]);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 8, FLOCK_SIZE);
        _frameIndex ^= 1;
    }

private:
    ShaderProgram _shader;
    ShaderProgram _compShader;

    GLuint _flockBuffer[2];
    GLuint _flockRenderVao[2];
    GLuint _geomBuffer;

    struct FlockMember
    {
        vmath::vec3 Position;
        unsigned int : 32;
        vmath::vec3 Velocity;
        unsigned int : 32;
    };

    struct
    {
        struct
        {
            GLint Goal;
        } Update;

        struct
        {
            GLuint MVP;
        } Render;
    } Uniforms;

    GLuint _frameIndex;
};

//DECLARE_MAIN(Flocking);

