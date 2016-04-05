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

#define FBO_SIZE 2048
#define FRUSTUM_DEPTH 1000


class DepthOfField : public sb6::application
{
public:
    DepthOfField() : _paused(false), _focalDistance(40.0f), _focalDepth(50.0f)
    {
    }

    void LoadShaders()
    {
        _compShader.CreateProgram();
        _compShader.AttachShader("Shaders/DepthOfField/DepthOfField.comp");
        _compShader.Link();

        _viewShader.CreateProgram();
        _viewShader.AttachShader("Shaders/DepthOfField/DepthOfFieldView.vert");
        _viewShader.AttachShader("Shaders/DepthOfField/DepthOfFieldView.frag");
        _viewShader.Link();

        Uniforms.View.Proj = glGetUniformLocation(_viewShader.GetHandler(), "proj_matrix");
        Uniforms.View.MV = glGetUniformLocation(_viewShader.GetHandler(), "mv_matrix");
        Uniforms.View.FullShading = glGetUniformLocation(_viewShader.GetHandler(), "full_shading");
        Uniforms.View.DiffuseAlbedo = glGetUniformLocation(_viewShader.GetHandler(), "diffuse_albedo");

        _displayShader.CreateProgram();
        _displayShader.AttachShader("Shaders/DepthOfField/DepthOfFieldDisplay.vert");
        _displayShader.AttachShader("Shaders/DepthOfField/DepthOfFieldDisplay.frag");
        _displayShader.Link();

        Uniforms.DoF.FocalDistanse = glGetUniformLocation(_displayShader.GetHandler(), "focal_distance");
        Uniforms.DoF.FocalDepth = glGetUniformLocation(_displayShader.GetHandler(), "focal_depth");
    }

    void startup()
    {
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
        LoadShaders();

        static const char* const objectNames[] =
            {
                "media/objects/dragon.sbm",
                "media/objects/sphere.sbm",
                "media/objects/cube.sbm",
                "media/objects/cube.sbm",
                "media/objects/cube.sbm"
            };
        static const vmath::vec4 objectColors[] =
            {
                vmath::vec4(1.0f, 0.7f, 0.8f, 1.0f),
                vmath::vec4(0.7f, 0.8f, 1.0f, 1.0f),
                vmath::vec4(0.3f, 0.9f, 0.4f, 1.0f),
                vmath::vec4(0.6f, 0.4f, 0.9f, 1.0f),
                vmath::vec4(0.8f, 0.2f, 0.1f, 1.0f),
            };
        for (int i = 0; i < ObjectCount; i++)
        {
            Objects[i].Obj.load(objectNames[i]);
            Objects[i].Diffuse = objectColors[i];
        }

        glGenFramebuffers(1, &_depthFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, _depthFBO);

        glGenTextures(1, &_depthTex);
        glBindTexture(GL_TEXTURE_2D, _depthTex);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, FBO_SIZE, FBO_SIZE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenTextures(1, &_colorTex);
        glBindTexture(GL_TEXTURE_2D, _colorTex);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, FBO_SIZE, FBO_SIZE);

        glGenTextures(1, &_tempTex);
        glBindTexture(GL_TEXTURE_2D, _tempTex);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, FBO_SIZE, FBO_SIZE);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthTex, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _colorTex, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glEnable(GL_DEPTH_TEST);
        glGenVertexArrays(1, &_quadVao);
        glBindVertexArray(_quadVao);
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void render(double currentTime)
    {
        static const GLfloat zeros[] = {0, 0, 0, 0};
        static double lastTime = 0.0;
        static double totalTime = 0.0;

        if (!_paused)
            totalTime += (currentTime - lastTime);
        lastTime = currentTime;

        const float f = (float)totalTime + 30.0f;

        vmath::vec3 view_position = vmath::vec3(0.0f, 0.0f, 40.0f);

        _cameraProj = vmath::perspective(50.0f,
                                         (float)info.windowWidth / (float)info.windowHeight,
                                         2.0f,
                                         300.0f);

        _cameraView = vmath::lookat(view_position,
                                    vmath::vec3(0.0f),
                                    vmath::vec3(0.0f, 1.0f, 0.0f));

        Objects[0].Model = vmath::translate(5.0f, 0.0f, 20.0f) *
            vmath::rotate(f * 14.5f, 0.0f, 1.0f, 0.0f) *
            vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f) *
            vmath::translate(0.0f, -4.0f, 0.0f);

        Objects[1].Model = vmath::translate(-5.0f, 0.0f, 0.0f) *
            vmath::rotate(f * 14.5f, 0.0f, 1.0f, 0.0f) *
            vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f) *
            vmath::translate(0.0f, -4.0f, 0.0f);

        Objects[2].Model = vmath::translate(-15.0f, 0.0f, -20.0f) *
            vmath::rotate(f * 14.5f, 0.0f, 1.0f, 0.0f) *
            vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f) *
            vmath::translate(0.0f, -4.0f, 0.0f);

        Objects[3].Model = vmath::translate(-25.0f, 0.0f, -40.0f) *
            vmath::rotate(f * 14.5f, 0.0f, 1.0f, 0.0f) *
            vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f) *
            vmath::translate(0.0f, -4.0f, 0.0f);

        Objects[4].Model = vmath::translate(-35.0f, 0.0f, -60.0f) *
            vmath::rotate(f * 14.5f, 0.0f, 1.0f, 0.0f) *
            vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f) *
            vmath::translate(0.0f, -4.0f, 0.0f);

        glEnable(GL_DEPTH_TEST);
        renderScene(totalTime);
        _compShader.Use();
        glBindImageTexture(0, _colorTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(1, _tempTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        glDispatchCompute(info.windowWidth, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glBindImageTexture(0, _tempTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(1, _colorTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        glDispatchCompute(info.windowWidth, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glBindTexture(GL_TEXTURE_2D, _colorTex);
        glDisable(GL_DEPTH_TEST);
        _displayShader.Use();
        glUniform1f(Uniforms.DoF.FocalDistanse, _focalDistance);
        glUniform1f(Uniforms.DoF.FocalDepth, _focalDepth);
        glBindVertexArray(_quadVao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void renderScene(double currentTime)
    {
        static const GLfloat ones[] = {1.0f};
        static const GLfloat zero[] = {0.0f};
        static const GLfloat gray[] = {0.1f, 0.1f, 0.1f, 0.0f};
        static const GLenum attachments[] = {GL_COLOR_ATTACHMENT0};
        glBindFramebuffer(GL_FRAMEBUFFER, _depthFBO);

        glDrawBuffers(1, attachments);
        glViewport(0, 0, info.windowWidth, info.windowHeight);
        glClearBufferfv(GL_COLOR, 0, gray);
        glClearBufferfv(GL_DEPTH, 0, ones);
        _viewShader.Use();
        glUniformMatrix4fv(Uniforms.View.Proj, 1, GL_FALSE, _cameraProj);
        glClearBufferfv(GL_DEPTH, 0, ones);
        for (int i = 0; i < ObjectCount; i++)
        {
            glUniformMatrix4fv(Uniforms.View.MV, 1, GL_FALSE, _cameraView * Objects[i].Model);
            glUniform3fv(Uniforms.View.DiffuseAlbedo, 1, Objects[i].Diffuse);
            //Objects[i].Obj.render();
            Objects[0].Obj.render();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void onKey(int key, int action)
    {
        if (action)
        {
            switch (key)
            {
            case 'Q':
                _focalDistance *= 1.1f;
                break;
            case'A':
                _focalDistance /= 1.1f;
                break;
            case 'W':
                _focalDepth *= 1.1f;
                break;
            case 'S':
                _focalDepth /= 1.1f;
                break;
            case 'P':
                _paused = !_paused;
                break;
            }
        }
    }

private:
    ShaderProgram _compShader;
    ShaderProgram _viewShader;
    ShaderProgram _displayShader;

    struct
    {
        struct
        {
            GLint FocalDistanse;
            GLint FocalDepth;
        } DoF;

        struct
        {
            GLint MV;
            GLint Proj;
            GLint FullShading;
            GLint DiffuseAlbedo;
        } View;
    } Uniforms;

    enum
    {
        ObjectCount = 5
    };

    struct
    {
        sb6::object Obj;
        vmath::mat4 Model;
        vmath::mat4 Diffuse;
    } Objects[ObjectCount];

    GLuint _depthFBO;
    GLuint _depthTex;
    GLuint _colorTex;
    GLuint _tempTex;

    vmath::mat4 _cameraView;
    vmath::mat4 _cameraProj;

    GLuint _quadVao;

    bool _paused;
    float _focalDistance;
    float _focalDepth;
};

DECLARE_MAIN(DepthOfField);

