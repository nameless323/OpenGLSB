#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"
#include "ShaderProgram.h"
#include <object.h>
#include <sb6ktx.h>

using namespace vmath;

static unsigned int seed = 0x13371337;

static inline float RandomFloat()
{
    float res;
    unsigned int tmp;

    seed *= 16807;

    tmp = seed ^ (seed >> 4) ^ (seed << 15);

    *((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

    return (res - 1.0f);
}

class SSAO : public sb6::application
{
public:
    SSAO() : _paused(false), _ssaoLevel(1.0f), _ssaoRadius(0.05f), _showShading(true), _showAO(true), _weightByAngle(true), _randomizePoints(true), _pointCount(10)
    {
    }

    void startup() override
    {
        loadShaders();

        glGenFramebuffers(1, &_renderFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, _renderFBO);
        glGenTextures(3, _FBOTextures);

        glBindTexture(GL_TEXTURE_2D, _FBOTextures[0]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, 2048, 2048);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, _FBOTextures[1]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 2048, 2048);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, _FBOTextures[2]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 2048, 2048);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _FBOTextures[0], 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _FBOTextures[1], 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _FBOTextures[2], 0);

        static const GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, drawBuffers);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenVertexArrays(1, &_quadVAO);
        glBindVertexArray(_quadVAO);

        _object.load("media/objects/dragon.sbm");
        _cube.load("media/objects/cube.sbm");

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        SAMPLE_POINTS pointData;

        for (int i = 0; i < 256; i++)
        {
            do
            {
                pointData.Point[i][0] = RandomFloat() * 2.0f - 1.0f;
                pointData.Point[i][1] = RandomFloat() * 2.0f - 1.0f;
                pointData.Point[i][2] = RandomFloat();
                pointData.Point[i][3] = 0.0f;
            }
            while (length(pointData.Point[i]) > 1.0f);
            normalize(pointData.Point[i]);
        }
        for (int i = 0; i < 256; i++)
        {
            pointData.RandomVectors[i][0] = RandomFloat();
            pointData.RandomVectors[i][1] = RandomFloat();
            pointData.RandomVectors[i][2] = RandomFloat();
            pointData.RandomVectors[i][3] = RandomFloat();
        }
        glGenBuffers(1, &_pointsBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER, _pointsBuffer);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(SAMPLE_POINTS), &pointData, GL_STATIC_DRAW);
    }

    void onResize(int w, int h) override
    {
        application::onResize(w, h);
    }

    void render(double currentTime) override
    {
        static const GLfloat black[] = {0.0f, 0.0f, 0.0f, 0.0f};
        static const GLfloat one = 1.0f;
        static double lastTime = 0.0;
        static double totalTime = 0.0;
        static const GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

        if (!_paused)
            totalTime += (currentTime - lastTime);
        lastTime = currentTime;

        float f = (float)totalTime;

        glViewport(0, 0, info.windowWidth, info.windowHeight);

        glBindFramebuffer(GL_FRAMEBUFFER, _renderFBO);
        glEnable(GL_DEPTH_TEST);
        glClearBufferfv(GL_COLOR, 0, black);
        glClearBufferfv(GL_COLOR, 1, black);
        glClearBufferfv(GL_DEPTH, 0, &one);

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _pointsBuffer);

        _renderShader.Use();

        const mat4 lookat = vmath::lookat(vec3(0.0f, 3.0f, 15.0f),
                                          vec3(0.0f, 0.0f, 0.0f),
                                          vec3(0.0f, 1.0f, 0.0f));

        mat4 proj_matrix = perspective(50.0f,
                                       (float)info.windowWidth / (float)info.windowHeight,
                                       0.1f,
                                       1000.0f);
        glUniformMatrix4fv(Uniforms.Render.Proj, 1, GL_FALSE, proj_matrix);

        mat4 modelMatrix = translate(0.0f, -5.0f, 0.0f) *
            rotate(f * 5.0f, 0.0f, 1.0f, 0.0f) *
            mat4::identity();
        glUniformMatrix4fv(Uniforms.Render.MV, 1, GL_FALSE, lookat * modelMatrix);

        glUniform1f(Uniforms.Render.ShadingLevel, _showShading ? (_showAO ? 0.7f : 1.0f) : 0.0f);

        _object.render();

        modelMatrix = translate(0.0f, -4.5f, 0.0f) *
            rotate(f * 5.0f, 0.0f, 1.0f, 0.0f) *
            scale(4000.0f, 0.1f, 4000.0f) *
            mat4::identity();
        glUniformMatrix4fv(Uniforms.Render.MV, 1, GL_FALSE, lookat * modelMatrix);

        _cube.render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        _SSAOShader.Use();

        glUniform1f(Uniforms.SSAOUniforms.SSAORadius, _ssaoRadius * float(info.windowWidth) / 1000.0f);
        glUniform1f(Uniforms.SSAOUniforms.SSAOLevel, _showAO ? (_showShading ? 0.3 : 1.0f) : 0.0f);
        glUniform1i(Uniforms.SSAOUniforms.RandomizePoints, _randomizePoints ? 1 : 0);
        glUniform1ui(Uniforms.SSAOUniforms.PointCount, _pointCount);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _FBOTextures[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _FBOTextures[1]);

        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(_quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void shutdown() override
    {
    }

    void loadShaders()
    {
        _renderShader.CreateProgram();
        _renderShader.AttachShader("Shaders/SSAO/Render.vert");
        _renderShader.AttachShader("Shaders/SSAO/Render.frag");
        _renderShader.Link();
        GLuint handler = _renderShader.GetHandler();
        Uniforms.Render.MV = glGetUniformLocation(handler, "mv_matrix");
        Uniforms.Render.Proj = glGetUniformLocation(handler, "proj_matrix");
        Uniforms.Render.ShadingLevel = glGetUniformLocation(handler, "shading_level");

        _SSAOShader.CreateProgram();
        _SSAOShader.AttachShader("Shaders/SSAO/SSAO.vert");
        _SSAOShader.AttachShader("Shaders/SSAO/SSAO.frag");
        _SSAOShader.Link();

        handler = _SSAOShader.GetHandler();
        Uniforms.SSAOUniforms.SSAORadius = glGetUniformLocation(handler, "ssao_radius");
        Uniforms.SSAOUniforms.SSAOLevel = glGetUniformLocation(handler, "ssao_level");
        Uniforms.SSAOUniforms.ObjectLevel = glGetUniformLocation(handler, "object_level");
        Uniforms.SSAOUniforms.WeightByAngle = glGetUniformLocation(handler, "weight_by_angle");
        Uniforms.SSAOUniforms.RandomizePoints = glGetUniformLocation(handler, "randomize_points");
        Uniforms.SSAOUniforms.PointCount = glGetUniformLocation(handler, "point_count");
    }

    void onKey(int key, int action) override
    {
        if (action)
        {
            switch (key)
            {
            case 'N':
                _weightByAngle = !_weightByAngle;
                break;
            case 'R':
                _randomizePoints = !_randomizePoints;
                break;
            case 'S':
                _pointCount++;
                break;
            case 'X':
                _pointCount--;
                break;
            case 'Q':
                _showShading = !_showShading;
                break;
            case 'W':
                _showAO = !_showAO;
                break;
            case 'A':
                _ssaoRadius += 0.01f;
                break;
            case 'Z':
                _ssaoRadius -= 0.01f;
                break;
            case 'P':
                _paused = !_paused;
                break;
            }
        }
    }

private:
    ShaderProgram _renderShader;
    ShaderProgram _SSAOShader;
    bool _paused;

    GLuint _renderFBO;
    GLuint _FBOTextures[3];
    GLuint _quadVAO;
    GLuint _pointsBuffer;

    sb6::object _object;
    sb6::object _cube;

    struct
    {
        struct
        {
            GLint MV;
            GLint Proj;
            GLint ShadingLevel;
        } Render;

        struct
        {
            GLint SSAOLevel;
            GLint ObjectLevel;
            GLint SSAORadius;
            GLint WeightByAngle;
            GLint RandomizePoints;
            GLint PointCount;
        } SSAOUniforms;
    } Uniforms;

    bool _showShading;
    bool _showAO;
    bool _ssaoLevel;
    bool _ssaoRadius;
    bool _weightByAngle;
    bool _randomizePoints;
    unsigned int _pointCount;

    struct SAMPLE_POINTS
    {
        vec4 Point[256];
        vec4 RandomVectors[256];
    };
};


DECLARE_MAIN(SSAO);
