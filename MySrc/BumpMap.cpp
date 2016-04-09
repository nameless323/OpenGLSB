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


class BumpMap : public sb6::application
{
public:
    BumpMap() : _paused(false)
    {
    }

    void LoadShaders()
    {
        _shader.CreateProgram();
        _shader.AttachShader("Shaders/BumpMap/BumpMap.vert");
        _shader.AttachShader("Shaders/BumpMap/BumpMap.frag");
        _shader.Link();

        Uniforms.MV = glGetUniformLocation(_shader.GetHandler(), "mv_matrix");
        Uniforms.Proj = glGetUniformLocation(_shader.GetHandler(), "proj_matrix");
        Uniforms.LightPos = glGetUniformLocation(_shader.GetHandler(), "light_pos");
    }

    void startup() override
    {
        LoadShaders();
        glActiveTexture(GL_TEXTURE0);
        Textures.Color = sb6::ktx::file::load("media/textures/ladybug_co.ktx");
        glActiveTexture(GL_TEXTURE1);
        Textures.Normals = sb6::ktx::file::load("media/textures/ladybug_nm.ktx");

        _object.load("media/objects/ladybug.sbm");
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void render(double currentTime) override
    {
        float t = (float)currentTime;
        static const float black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        static const float green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
        static const float one = 1.0f;
        static double lastTime = 0.0;
        static double totalTime = 0.0;

        if (!_paused)
            totalTime += (currentTime - lastTime);
        lastTime = currentTime;
        float f = (float)totalTime;

        glViewport(0, 0, info.windowWidth, info.windowHeight);
        glClearBufferfv(GL_COLOR, 0, black);
        glClearBufferfv(GL_DEPTH, 0, &one);
        _shader.Use();

        vmath::mat4 proj = vmath::perspective(50.0f,
            (float)info.windowWidth / (float)info.windowHeight,
            0.1f,
            1000.0f);
        glUniformMatrix4fv(Uniforms.Proj, 1, GL_FALSE, proj);
        vmath::mat4 mv = vmath::translate(0.0f, -0.2f, -5.5f) *
            vmath::rotate(14.5f, 1.0f, 0.0f, 0.0f) *
            vmath::rotate(-20.0f, 0.0f, 1.0f, 0.0f);
        glUniformMatrix4fv(Uniforms.MV, 1, GL_FALSE, mv);

        glUniform3fv(Uniforms.LightPos, 1, vmath::vec3(40.0f * sinf(f), 30.0f + 20.0f * cosf(f), 40.0f));
        _object.render();
    }

    void onKey(int key, int action)
    {
        if (action)
        {
            switch (key)
            {
            case 'P':
                _paused = !_paused;
                break;
            }
        }
    }

private:
    ShaderProgram _shader;

    struct
    {
        GLuint MV;
        GLuint Proj;
        GLuint LightPos;
    } Uniforms;

    struct
    {
        GLuint Color;
        GLuint Normals;
    } Textures;

    vmath::mat4 _rot;
    sb6::object _object;
    vmath::vec3 _rimColor;
    float _rimPower;
    bool _rimEnable;
    bool _paused;
};

DECLARE_MAIN(BumpMap);

