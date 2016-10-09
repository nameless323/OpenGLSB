#include <sb6.h>
#include <sb6ktx.h>
#include <object.h>

#include "Utils.h"
#include "vmath.h"
#include "ShaderProgram.h"

namespace OpenGlSB
{
class SphereEnvMap : public sb6::application
{
public:
    SphereEnvMap() : _envMapIndex(0)
    {
    }

    void LoadShaders()
    {
        _shader.CreateProgram();
        _shader.AttachShader("Shaders/SphereEnvMap/SphereEnvMap.vert");
        _shader.AttachShader("Shaders/SphereEnvMap/SphereEnvMap.frag");
        _shader.Link();

        Uniforms.MV = glGetUniformLocation(_shader.GetHandler(), "mv_matrix");
        Uniforms.Proj = glGetUniformLocation(_shader.GetHandler(), "proj_matrix");
    }

    void startup() override
    {
        LoadShaders();
        _envMaps[0] = sb6::ktx::file::load("media/textures/envmaps/spheremap1.ktx");
        _envMaps[1] = sb6::ktx::file::load("media/textures/envmaps/spheremap2.ktx");
        _envMaps[2] = sb6::ktx::file::load("media/textures/envmaps/spheremap3.ktx");
        _texEnvMap = _envMaps[_envMapIndex];

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


        glViewport(0, 0, info.windowWidth, info.windowHeight);
        glClearBufferfv(GL_COLOR, 0, black);
        glClearBufferfv(GL_DEPTH, 0, &one);
        glBindTexture(GL_TEXTURE_2D, _texEnvMap);
        _shader.Use();

        vmath::mat4 proj = vmath::perspective(60.0f,
                                              (float)info.windowWidth / (float)info.windowHeight,
                                              0.1f,
                                              1000.0f);
        glUniformMatrix4fv(Uniforms.Proj, 1, GL_FALSE, proj);
        vmath::mat4 mv = vmath::translate(0.0f, 0.0f, -15.0f) *
                vmath::rotate((float)currentTime, 1.0f, 0.0f, 0.0f) *
                vmath::rotate((float)currentTime * 1.1f, 0.0f, 1.0f, 0.0f) *
                vmath::translate(0.0f, -4.0f, 0.0f);
        glUniformMatrix4fv(Uniforms.MV, 1, GL_FALSE, mv);

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

    struct
    {
        GLuint MV;
        GLuint Proj;
    } Uniforms;

    GLuint _texEnvMap;
    GLuint _envMaps[3];
    int _envMapIndex;
    sb6::object _object;
};
}
