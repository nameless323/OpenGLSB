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


class RimLight : public sb6::application
{
public:
    RimLight() : _paused(false), _rimColor(0.3f, 0.3f, 0.3f), _rimPower(2.5f), _rimEnable(true)
    {
    }

    void LoadShaders()
    {
        _shader.CreateProgram();
        _shader.AttachShader("Shaders/RimLight/RimLight.vert");
        _shader.AttachShader("Shaders/RimLight/RimLight.frag");
        _shader.Link();

        Uniforms.MV = glGetUniformLocation(_shader.GetHandler(), "mv_matrix");
        Uniforms.Proj = glGetUniformLocation(_shader.GetHandler(), "proj_matrix");
        Uniforms.RimCol = glGetUniformLocation(_shader.GetHandler(), "rim_color");
        Uniforms.RimPow = glGetUniformLocation(_shader.GetHandler(), "rim_power");
    }

    void startup() override
    {
        LoadShaders();
        _object.load("media/objects/dragon.sbm");
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void render(double currentTime) override
    {
        float t = (float)currentTime;
        static const float black[] = {0.0f, 0.0f, 0.0f, 1.0f};
        static const float green[] = {0.0f, 0.25f, 0.0f, 1.0f};
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
        vmath::mat4 mv = vmath::translate(0.0f, -5.0f, -20.0f) *
            vmath::rotate(f * 5.0f, 0.0f, 1.0f, 0.0f);
        glUniformMatrix4fv(Uniforms.MV, 1, GL_FALSE, mv);

        glUniform3fv(Uniforms.RimCol, 1, _rimEnable ? _rimColor : vmath::vec3(0.0f));
        glUniform1f(Uniforms.RimPow, _rimPower);
        _object.render();
    }

    void onKey(int key, int action)
    {
        if (action)
        {
            switch (key)
            {
            case 'Q':
                _rimColor[0] += 0.1f;
                break;
            case 'W':
                _rimColor[1] += 0.1f;
                break;
            case 'E':
                _rimColor[2] += 0.1f;
                break;
            case 'R':
                _rimPower *= 1.5f;
                break;
            case 'A':
                _rimColor[0] -= 0.1f;
                break;
            case 'S':
                _rimColor[1] -= 0.1f;
                break;
            case 'D':
                _rimColor[2] -= 0.1f;
                break;
            case 'F':
                _rimPower /= 1.5f;
                break;
            case 'Z':
                _rimEnable = !_rimEnable;
                break;

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
        GLuint RimCol;
        GLuint RimPow;
    } Uniforms;

    vmath::mat4 _rot;
    sb6::object _object;
    vmath::vec3 _rimColor;
    float _rimPower;
    bool _rimEnable;
    bool _paused;
};

//DECLARE_MAIN(RimLight);

