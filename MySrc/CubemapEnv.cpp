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


class CubemapEnv : public sb6::application
{
public:
    CubemapEnv() : _envMapIndex(0)
    {
    }

    void LoadShaders()
    {
        _shader.CreateProgram();
        _shader.AttachShader("Shaders/Cubemap/Cubemap.vert");
        _shader.AttachShader("Shaders/Cubemap/Cubemap.frag");
        _shader.Link();

        Uniforms.MV = glGetUniformLocation(_shader.GetHandler(), "mv_matrix");
        Uniforms.Proj = glGetUniformLocation(_shader.GetHandler(), "proj_matrix");

        _skyboxShader.CreateProgram();
        _skyboxShader.AttachShader("Shaders/Cubemap/Skybox.vert");
        _skyboxShader.AttachShader("Shaders/Cubemap/Skybox.frag");
        _skyboxShader.Link();

        Uniforms.View = glGetUniformLocation(_skyboxShader.GetHandler(), "view_matrix");
    }

    void startup() override
    {
        LoadShaders();
        _envMaps[0] = sb6::ktx::file::load("media/textures/envmaps/mountaincube.ktx");
        _texEnvMap = _envMaps[_envMapIndex];
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        _object.load("media/objects/dragon.sbm");
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glGenVertexArrays(1, &_skyboxVAO);
        glBindVertexArray(_skyboxVAO);
    }

    void render(double currentTime) override
    {
        float t = (float)currentTime;
        static const float black[] = {0.0f, 0.0f, 0.0f, 1.0f};
        static const float green[] = {0.0f, 0.25f, 0.0f, 1.0f};
        static const float one = 1.0f;
        static double lastTime = 0.0;
        static double totalTime = 0.0;


        glViewport(0, 0, info.windowWidth, info.windowHeight);
        glClearBufferfv(GL_COLOR, 0, black);
        glClearBufferfv(GL_DEPTH, 0, &one);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _texEnvMap);

        vmath::mat4 proj = vmath::perspective(60.0f,
                                              (float)info.windowWidth / (float)info.windowHeight,
                                              0.1f,
                                              1000.0f);
        vmath::mat4 mv = vmath::translate(0.0f, 0.0f, -15.0f) *
            vmath::rotate((float)currentTime, 1.0f, 0.0f, 0.0f) *
            vmath::rotate((float)currentTime * 1.1f, 0.0f, 1.0f, 0.0f) *
            vmath::translate(0.0f, -4.0f, 0.0f);
        vmath::mat4 view = lookat(vmath::vec3(15.0f * sinf(t), 0.0f, 15.0f * cosf(t)),
                                         vmath::vec3(0.0f, 0.0f, 0.0f),
                                         vmath::vec3(0.0f, 1.0f, 0.0f));
        _skyboxShader.Use();
        glUniformMatrix4fv(Uniforms.View, 1, GL_FALSE, view);
        glBindVertexArray(_skyboxVAO);
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        _shader.Use();
        glUniformMatrix4fv(Uniforms.Proj, 1, GL_FALSE, proj);
        glUniformMatrix4fv(Uniforms.MV, 1, GL_FALSE, mv);
        glEnable(GL_DEPTH_TEST);


        _object.render();

    }

    void onKey(int key, int action)
    {
        if (action)
        {
            switch (key)
            {
            case 'E':
                _envMapIndex = (_envMapIndex + 1) % 3;
                _texEnvMap = _envMaps[_envMapIndex];
                break;
            }
        }
    }

private:
    ShaderProgram _shader;
    ShaderProgram _skyboxShader;
    GLuint _skyboxVAO;

    struct
    {
        GLuint MV;
        GLuint Proj;
        GLuint View;
    } Uniforms;

    GLuint _texEnvMap;
    GLuint _envMaps[3];
    int _envMapIndex;
    sb6::object _object;
};

DECLARE_MAIN(CubemapEnv);

