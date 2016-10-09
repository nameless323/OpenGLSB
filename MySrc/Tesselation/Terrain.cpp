//
// Tesselation based terrain.
//

#include <sb6.h>

#include <sb6ktx.h>

#include "vmath.h"
#include "../ShaderProgram.h"

namespace OpenGlSB
{
using vmath::mat4;
using vmath::vec3;

class Terrain : public sb6::application
{
public:
    Terrain() : _enableDisplacement(true), _wireframe(false), _enableFog(true), _paused(false)
    {
    }

    void startup()
    {
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
        _renderingProgram.CreateProgram();
        _renderingProgram.AttachShader("Shaders/Tesselation/Terrain/Terrain.vert");
        _renderingProgram.AttachShader("Shaders/Tesselation/Terrain/Terrain.tesc");
        _renderingProgram.AttachShader("Shaders/Tesselation/Terrain/Terrain.tese");
        _renderingProgram.AttachShader("Shaders/Tesselation/Terrain/Terrain.frag");
        _renderingProgram.Link();

        _uniforms.MV = glGetUniformLocation(_renderingProgram.GetHandler(), "mv_matrix");
        _uniforms.MVP = glGetUniformLocation(_renderingProgram.GetHandler(), "mvp_matrix");
        _uniforms.Proj = glGetUniformLocation(_renderingProgram.GetHandler(), "proj_matrix");
        _uniforms.DispDepth = glGetUniformLocation(_renderingProgram.GetHandler(), "dmap_depth");
        _uniforms.EnableFog = glGetUniformLocation(_renderingProgram.GetHandler(), "enable_fog");
        _displacementDepth = 6.0f;

        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        glPatchParameteri(GL_PATCH_VERTICES, 4);
        glEnable(GL_CULL_FACE);

        _displacementTex = sb6::ktx::file::load("media/textures/terragen1.ktx");
        glActiveTexture(GL_TEXTURE1);
        _mainTex = sb6::ktx::file::load("media/textures/terragen_color.ktx");

        _renderingProgram.Use();
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void render(double currentTime)
    {
        const GLfloat bckColor[] = {0.85f, 0.95f, 1.0f, 1.0f};
        const GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};

        static double lastTime = 0.0;
        static double totalTime = 0.0;

        if (!_paused)
            totalTime += (currentTime - lastTime);
        lastTime = currentTime;

        float t = (float)totalTime * 0.03f;
        float r = sinf(t * 5.37f) * 15.0f + 16.0f;
        float h = cosf(t * 4.79f) * 2.0f + 3.2f;

        glViewport(0, 0, info.windowWidth, info.windowHeight);

        glClearBufferfv(GL_COLOR, 0, bckColor);
        glClearBufferfv(GL_DEPTH, 0, white);

        mat4 mv = lookat(vec3(sinf(t) * r, h, cosf(t) * r), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
        mat4 proj = vmath::perspective(60.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);
        _renderingProgram.Use();

        glUniformMatrix4fv(_uniforms.MV, 1, GL_FALSE, mv);
        glUniformMatrix4fv(_uniforms.Proj, 1, GL_FALSE, proj);
        glUniformMatrix4fv(_uniforms.MVP, 1, GL_FALSE, proj * mv);
        glUniform1f(_uniforms.DispDepth, _enableDisplacement ? _displacementDepth : 0.0f);
        glUniform1i(_uniforms.EnableFog, _enableFog ? 1 : 0);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        if (_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
    }

    void shutdown()
    {
        _renderingProgram.DeleteProgram();

        glDisableVertexAttribArray(0);

        glBindVertexArray(0);
        glDeleteVertexArrays(1, &_vao);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void onKey(int key, int action)
    {
        if (action == 1)
        {
            switch (key)
            {
                case GLFW_KEY_KP_ADD: _displacementDepth += 0.1f;
                    break;
                case GLFW_KEY_KP_SUBTRACT: _displacementDepth -= 0.1f;
                    break;
                case 'F': _enableFog = !_enableFog;
                    break;
                case 'D': _enableDisplacement = !_enableDisplacement;
                    break;
                case 'W': _wireframe = !_wireframe;
                    break;
                default: break;
            }
        }
    }

private:
    ShaderProgram _renderingProgram;
    GLuint _vao;
    GLuint _displacementTex;
    GLuint _mainTex;

    float _displacementDepth;
    bool _enableDisplacement;
    bool _wireframe;
    bool _enableFog;
    bool _paused;

    struct
    {
        GLint MVP;
        GLint Proj;
        GLint MV;
        GLint DispDepth;
        GLint EnableFog;
    } _uniforms;
};
}
