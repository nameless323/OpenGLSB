#include <sb6.h>
#include <cmath>
#include <object.h>
#include <sb6ktx.h>

#include "Utils.h"
#include "vmath.h"
#include "ShaderProgram.h"

namespace OpenGlSB
{
using namespace vmath;

enum
{
    MAX_DISPALY_WIDTH = 2048,
    MAX_DISPLAY_HEIGHT = 2048,
    NUM_LIGHTS = 64,
    NUM_INSTANCES = (15 * 15)
};

class DeferredShading : public sb6::application
{
public:
    DeferredShading() : _useNM(true), _paused(false), vis_mode(VIS_OFF)
    {
    }

    void startup() override
    {
        glGenFramebuffers(1, &_GBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _GBuffer);

        glGenTextures(3, _GBufferTex);
        glBindTexture(GL_TEXTURE_2D, _GBufferTex[0]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32UI, MAX_DISPALY_WIDTH, MAX_DISPLAY_HEIGHT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, _GBufferTex[1]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, MAX_DISPALY_WIDTH, MAX_DISPLAY_HEIGHT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, _GBufferTex[2]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, MAX_DISPALY_WIDTH, MAX_DISPLAY_HEIGHT);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _GBufferTex[0], 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _GBufferTex[1], 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _GBufferTex[2], 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenVertexArrays(1, &_quadVAO);
        glBindVertexArray(_quadVAO);

        _object.load("media/objects/ladybug.sbm");
        _nmTex = sb6::ktx::file::load("media/textures/ladybug_nm.ktx");
        _diffuseTex = sb6::ktx::file::load("media/textures/ladybug_co.ktx");

        loadShaders();

        glGenBuffers(1, &_lightUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, _lightUBO);
        glBufferData(GL_UNIFORM_BUFFER, NUM_LIGHTS * sizeof(LightT), nullptr, GL_DYNAMIC_DRAW);

        glGenBuffers(1, &_renderTransformUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, _renderTransformUBO);
        glBufferData(GL_UNIFORM_BUFFER, (2 + NUM_INSTANCES) * sizeof(mat4), nullptr, GL_DYNAMIC_DRAW);
    }

    void onResize(int w, int h) override
    {
        application::onResize(w, h);
    }

    void render(double currentTime) override
    {
        static const GLuint uintZeros[] = {0, 0, 0, 0};
        static const GLfloat floatZeros[] = {0.0f, 0.0f, 0.0f, 0.0f};
        static const GLfloat floatOnes[] = {1.0f, 1.0f, 1.0f, 1.0f};
        static const GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        static double lastTime = 0.0f;
        static double totalTime = 0.0f;

        if (!_paused)
            totalTime += (currentTime - lastTime);
        else
            Sleep(10);

        lastTime = currentTime;
        float t = (float)totalTime;
        glBindFramebuffer(GL_FRAMEBUFFER, _GBuffer);
        glViewport(0, 0, info.windowWidth, info.windowHeight);
        glDrawBuffers(2, drawBuffers);
        glClearBufferuiv(GL_COLOR, 0, uintZeros);
        glClearBufferuiv(GL_COLOR, 1, uintZeros);
        glClearBufferfv(GL_DEPTH, 0, floatOnes);

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _renderTransformUBO);
        mat4* matrices = (mat4*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, (2 + NUM_INSTANCES) * sizeof(mat4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        matrices[0] = perspective(50.0f,
                                  (float)info.windowWidth / (float)info.windowHeight,
                                  0.1f,
                                  1000.0f);
        float d = (sinf(t * 0.131f) + 2.0f) * 0.15f;
        vec3 eyePos = vec3(d * 120.0f * sinf(t * 0.11f),
                           5.5f,
                           d * 120.0f * cosf(t * 0.01f));
        matrices[1] = lookat(eyePos,
                             vec3(0.0f, -20.0f, 0.0f),
                             vec3(0.0f, 1.0f, 0.0f));
        for (int j = 0; j < 15; j++)
        {
            for (int i = 0; i < 15; i++)
            {
                matrices[j * 15 + i + 2] = translate((i - 7.5f) * 7.0f, 0.0f, (j - 7.5f) * 11.0f);
            }
        }
        glUnmapBuffer(GL_UNIFORM_BUFFER);

        if (_useNM)
            _nmShader.Use();
        else
            _renderingShader.Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _diffuseTex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _nmTex);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        _object.render(NUM_INSTANCES);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, info.windowWidth, info.windowHeight);
        glDrawBuffer(GL_BACK);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _GBufferTex[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _GBufferTex[1]);

        if (vis_mode == VIS_OFF)
            _lightShader.Use();
        else
        {
            _visShader.Use();
            glUniform1i(_locVisMode, vis_mode);
        }
        glDisable(GL_DEPTH_TEST);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _lightUBO);
        LightT* lights = (LightT*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, NUM_LIGHTS * sizeof(LightT), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        for (int i = 0; i < NUM_LIGHTS; i++)
        {
            float iFloat = ((float)i - 7.5f) * 0.1f + 0.3f;
            lights[i].Position = vmath::vec3(100.0f * sinf(t * 1.1f + (5.0f * iFloat)) * cosf(t * 2.3f + (9.0f * iFloat)),
                15.0f,
                100.0f * sinf(t * 1.5f + (6.0f * iFloat)) * cosf(t * 1.9f + (11.0f * iFloat))); // 300.0f * sinf(t * i_f * 0.7f) * cosf(t * i_f * 0.9f) - 600.0f);
            lights[i].Color = vmath::vec3(cosf(iFloat * 14.0f) * 0.5f + 0.8f,
                sinf(iFloat * 17.0f) * 0.5f + 0.8f,
                sinf(iFloat * 13.0f) * cosf(iFloat * 19.0f) * 0.5f + 0.8f);
        }
        glUnmapBuffer(GL_UNIFORM_BUFFER);

        glBindVertexArray(_quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void shutdown() override
    {
        glDeleteTextures(3, _GBufferTex);
        glDeleteFramebuffers(1, &_GBuffer);
        _renderingShader.DeleteProgram();
        _lightShader.DeleteProgram();
        _nmShader.DeleteProgram();
        _visShader.DeleteProgram();
    }

    void loadShaders()
    {
        _renderingShader.CreateProgram();
        _renderingShader.AttachShader("Shaders/DeferredShading/Render.vert");
        _renderingShader.AttachShader("Shaders/DeferredShading/Render.frag");
        _renderingShader.Link();

        _nmShader.CreateProgram();
        _nmShader.AttachShader("Shaders/DeferredShading/RenderNM.vert");
        _nmShader.AttachShader("Shaders/DeferredShading/RenderNM.frag");
        _nmShader.Link();

        _lightShader.CreateProgram();
        _lightShader.AttachShader("Shaders/DeferredShading/Light.vert");
        _lightShader.AttachShader("Shaders/DeferredShading/Light.frag");
        _lightShader.Link();

        _visShader.CreateProgram();
        _visShader.AttachShader("Shaders/DeferredShading/Light.vert");
        _visShader.AttachShader("Shaders/DeferredShading/RenderVIS.frag");
        _visShader.Link();

        _locVisMode = glGetUniformLocation(_visShader.GetHandler(), "vis_mode");
    }

    void onKey(int key, int action) override
    {
        if (action)
        {
            switch (key)
            {
            case 'P': _paused = !_paused;
                break;
            case 'N': _useNM = !_useNM;
                break;
            case '1': vis_mode = VIS_OFF;
                break;
            case '2': vis_mode = VIS_NORMALS;
                break;
            case '3': vis_mode = VIS_WS_COORDS;
                break;
            case '4': vis_mode = VIS_DIFFUSE;
                break;
            case '5': vis_mode = VIS_META;
                break;
            }
        }
    }

private:
    GLuint _GBuffer;
    GLuint _GBufferTex[3];
    GLuint _quadVAO;

    sb6::object _object;

    ShaderProgram _renderingShader;
    ShaderProgram _nmShader;
    ShaderProgram _transformUBOShader;

    ShaderProgram _lightShader;
    GLuint _lightUBO;

    ShaderProgram _visShader;
    GLint _locVisMode;

    GLuint _diffuseTex;
    GLuint _nmTex;

    GLuint _renderTransformUBO;

    bool _useNM;
    bool _paused;

    enum
    {
        VIS_OFF,
        VIS_NORMALS,
        VIS_WS_COORDS,
        VIS_DIFFUSE,
        VIS_META
    } vis_mode;

#pragma pack (push, 1)
    struct LightT
    {
        vec3 Position;
        unsigned int : 32;
        vec3 Color;
        unsigned int : 32;
    };
#pragma pack (pop)
};
}
