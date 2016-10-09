#include <sb6.h>

#include <cmath>

#include "Utils.h"
#include "vmath.h"
#include "ShaderProgram.h"

namespace OpenGlSB
{
using namespace vmath;

class Raytracer : public sb6::application
{
public:
    Raytracer() : _maxDepth(1), _debugDepth(0), _debugMode(DEBUG_NONE), _paused(false)
    {
    }

    void LoadShaders()
    {
        _prepareShader.CreateProgram();
        _prepareShader.AttachShader("Shaders/Raytracer/Prepare.vert");
        _prepareShader.AttachShader("Shaders/Raytracer/Prepare.frag");
        _prepareShader.Link();
        Uniforms.RayOrigin = glGetUniformLocation(_prepareShader.GetHandler(), "ray_origin");
        Uniforms.RayLookat = glGetUniformLocation(_prepareShader.GetHandler(), "ray_lookat");
        Uniforms.Aspect = glGetUniformLocation(_prepareShader.GetHandler(), "aspect");

        _raytracerShader.CreateProgram();
        _raytracerShader.AttachShader("Shaders/Raytracer/Raytracer.vert");
        _raytracerShader.AttachShader("Shaders/Raytracer/Raytracer.frag");
        _raytracerShader.Link();

        _blitShader.CreateProgram();
        _blitShader.AttachShader("Shaders/Raytracer/Blit.vert");
        _blitShader.AttachShader("Shaders/Raytracer/Blit.frag");
        _blitShader.Link();
    }

    void startup()
    {
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
        LoadShaders();
        glGenBuffers(1, &_uniformsBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER, _uniformsBuffer);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformsBlock), nullptr, GL_DYNAMIC_DRAW);

        glGenBuffers(1, &_sphereBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER, _sphereBuffer);
        glBufferData(GL_UNIFORM_BUFFER, 128 * sizeof(Sphere), nullptr, GL_DYNAMIC_DRAW);

        glGenBuffers(1, &_planeBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER, _planeBuffer);
        glBufferData(GL_UNIFORM_BUFFER, 128 * sizeof(Plane), nullptr, GL_DYNAMIC_DRAW);

        glGenBuffers(1, &_lightBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER, _lightBuffer);
        glBufferData(GL_UNIFORM_BUFFER, 128 * sizeof(Light), nullptr, GL_DYNAMIC_DRAW); // o.O

        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        glGenFramebuffers(MAX_RECURSION_DEPTH, _rayFBO);
        glGenTextures(1, &_texComposite);
        glGenTextures(MAX_RECURSION_DEPTH, _texPosition);
        glGenTextures(MAX_RECURSION_DEPTH, _texReflected);
        glGenTextures(MAX_RECURSION_DEPTH, _texRefracted);
        glGenTextures(MAX_RECURSION_DEPTH, _texReflectionIntensity);
        glGenTextures(MAX_RECURSION_DEPTH, _texRefractionIntensity);

        glBindTexture(GL_TEXTURE_2D, _texComposite);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);

        for (int i = 0; i < MAX_RECURSION_DEPTH; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, _rayFBO[i]);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _texComposite, 0);

            glBindTexture(GL_TEXTURE_2D, _texPosition[i]);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _texPosition[i], 0);

            glBindTexture(GL_TEXTURE_2D, _texReflected[i]);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, _texReflected[i], 0);

            glBindTexture(GL_TEXTURE_2D, _texRefracted[i]);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, _texRefracted[i], 0);

            glBindTexture(GL_TEXTURE_2D, _texReflectionIntensity[i]);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, _texReflectionIntensity[i], 0);

            glBindTexture(GL_TEXTURE_2D, _texRefractionIntensity[i]);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, _texRefractionIntensity[i], 0);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
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
        static double lastTime = 0.0;
        static double totalTime = 0.0;

        if (!_paused)
            totalTime += currentTime - lastTime;
        lastTime = currentTime;
        float f = (float)totalTime;

        vec3 viewPosition = vec3(sinf(f * 0.3234f) * 28.0f, cosf(f * 0.4234f) * 28.0f, cosf(f * 0.1234f) * 28.0f); // sinf(f * 0.2341f) * 20.0f - 8.0f);
        vec3 lookatPoint = vec3(sinf(f * 0.214f) * 8.0f, cosf(f * 0.153f) * 8.0f, sinf(f * 0.734f) * 8.0f);
        mat4 view = lookat(viewPosition,
                           lookatPoint,
                           vec3(0.0f, 1.0f, 0.0f));

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _uniformsBuffer);
        UniformsBlock* block = (UniformsBlock*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(UniformsBlock), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        mat4 model = scale(7.0f);

        block->MV = view * model;
        block->View = view;
        block->Proj = vmath::perspective(50.0f,
                                         (float)info.windowWidth / (float)info.windowHeight,
                                         0.1f,
                                         1000.0f);
        glUnmapBuffer(GL_UNIFORM_BUFFER);

        glBindBufferBase(GL_UNIFORM_BUFFER, 1, _sphereBuffer);
        Sphere* s = (Sphere*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 128 * sizeof(Sphere), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        for (int i = 0; i < 128; i++)
        {
            float fi = (float)i / 128.0f;
            s[i].Center =  vmath::vec3(sinf(fi * 123.0f + f) * 15.75f, cosf(fi * 456.0f + f) * 15.75f, (sinf(fi * 300.0f + f) * cosf(fi * 200.0f + f)) * 20.0f);
            s[i].Radius = fi * 2.3f + 3.5f;
            float r = fi * 61.0f;
            float g = r + 0.25f;
            float b = g + 0.25f;
            r = (r - floorf(r)) * 0.8f + 0.2f;
            g = (g - floorf(g)) * 0.8f + 0.2f;
            b = (b - floorf(b)) * 0.8f + 0.2f;
            s[i].Color = vec4(r, g, b, 1.0f);
        }

        glUnmapBuffer(GL_UNIFORM_BUFFER);
        glBindBufferBase(GL_UNIFORM_BUFFER, 2, _planeBuffer);
        Plane* p = (Plane*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 128 * sizeof(Plane), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        p[0].Normal = vec3(0.0f, 0.0f, -1.0f);
        p[0].D = 30.0f;

        p[1].Normal = vec3(0.0f, 0.0f, 1.0f);
        p[1].D = 30.0f;

        p[2].Normal = vec3(-1.0f, 0.0f, 0.0f);
        p[2].D = 30.0f;

        p[3].Normal = vec3(1.0f, 0.0f, 0.0f);
        p[3].D = 30.0f;

        p[4].Normal = vec3(0.0f, -1.0f, 0.0f);
        p[4].D = 30.0f;

        p[5].Normal = vec3(0.0f, 1.0f, 0.0f);
        p[5].D = 30.0f;

        glUnmapBuffer(GL_UNIFORM_BUFFER);
        glBindBufferBase(GL_UNIFORM_BUFFER, 3, _lightBuffer);
        Light* l = (Light*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 128 * sizeof(Light), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        for (int i = 0; i < 128; i++)
        {
            float fi = 3.33f - (float)i; //  / 35.0f;
            l[i].Position = vec3(sinf(fi * 2.0f - f) * 15.75f,
                cosf(fi * 5.0f - f) * 5.75f,
                (sinf(fi * 3.0f - f) * cosf(fi * 2.5f - f)) * 19.4f);
        }
        glUnmapBuffer(GL_UNIFORM_BUFFER);

        glBindVertexArray(_vao);
        glViewport(0, 0, info.windowWidth, info.windowHeight);

        _prepareShader.Use();
        glUniformMatrix4fv(Uniforms.RayLookat, 1, GL_FALSE, view);
        glUniform3fv(Uniforms.RayOrigin, 1, viewPosition);
        glUniform1f(Uniforms.Aspect, (float)info.windowHeight / (float)info.windowWidth);
        glBindFramebuffer(GL_FRAMEBUFFER, _rayFBO[0]);
        static const GLenum drawBuffers[] =
        {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2,
            GL_COLOR_ATTACHMENT3,
            GL_COLOR_ATTACHMENT4,
            GL_COLOR_ATTACHMENT5
        };
        glDrawBuffers(6, drawBuffers);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        _raytracerShader.Use();
        recurce(0);

        _blitShader.Use();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDrawBuffer(GL_BACK);

        glActiveTexture(GL_TEXTURE0);
        switch (_debugMode)
        {
            case DEBUG_NONE:
                glBindTexture(GL_TEXTURE_2D, _texComposite);
                break;
            case DEBUG_REFLECTED:
                glBindTexture(GL_TEXTURE_2D, _texReflected[_debugDepth]);
                break;;
            case DEBUG_REFRACTED:
                glBindTexture(GL_TEXTURE_2D, _texReflected[_debugDepth]);
                break;
            case DEBUG_REFLECTED_COLOR:
                glBindTexture(GL_TEXTURE_2D, _texReflectionIntensity[_debugDepth]);
                break;
            case DEBUG_REFRACTED_COLOR:
                glBindTexture(GL_TEXTURE_2D, _texRefractionIntensity[_debugDepth]);
                break;
        }
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void recurce(int depth)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _rayFBO[depth + 1]);

        static const GLenum drawBuffers[] =
            {
                GL_COLOR_ATTACHMENT0,
                GL_COLOR_ATTACHMENT1,
                GL_COLOR_ATTACHMENT2,
                GL_COLOR_ATTACHMENT3,
                GL_COLOR_ATTACHMENT4,
                GL_COLOR_ATTACHMENT5
            };
        glDrawBuffers(6, drawBuffers);

        glEnablei(GL_BLEND, 0);
        glBlendFunci(0, GL_ONE, GL_ONE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texPosition[depth]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _texReflected[depth]);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, _texReflectionIntensity[depth]);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        if (depth != (_maxDepth - 1))
            recurce(depth + 1);
        glDisablei(GL_BLEND, 0);
    }

    void shutdown()
    {
        _prepareShader.DeleteProgram();
        _blitShader.DeleteProgram();
        _raytracerShader.DeleteProgram();

        glDisableVertexAttribArray(0);

        glBindVertexArray(0);
        glDeleteVertexArrays(1, &_vao);
    }

    void onKey(int key, int action)
    {
        if (action)
        {
            switch (key)
            {
            case GLFW_KEY_KP_ADD:
                _maxDepth++;
                if (_maxDepth > MAX_RECURSION_DEPTH)
                    _maxDepth = MAX_RECURSION_DEPTH;
                break;
            case GLFW_KEY_KP_SUBTRACT:
                _maxDepth--;
                if (_maxDepth < 1)
                    _maxDepth = 1;
                break;
            case 'P':
                _paused = !_paused;
                break;
            case 'Q':
                _debugMode = DEBUG_NONE;
                break;
            case 'W':
                _debugMode = DEBUG_REFLECTED;
                break;
            case 'E':
                _debugMode = DEBUG_REFRACTED;
                break;
            case 'S':
                _debugMode = DEBUG_REFLECTED_COLOR;
                break;
            case 'D':
                _debugMode = DEBUG_REFRACTED_COLOR;
                break;
            case 'A':
                _debugDepth++;
                if (_debugDepth > MAX_RECURSION_DEPTH)
                    _debugDepth = MAX_RECURSION_DEPTH;
                break;
            case 'Z':
                _debugDepth--;
                if (_debugDepth < 0)
                    _debugDepth = 0;
                break;
            }
        }
    }

private:
    ShaderProgram _prepareShader;
    ShaderProgram _raytracerShader;
    ShaderProgram _blitShader;

    struct UniformsBlock
    {
        mat4 MV;
        mat4 View;
        mat4 Proj;
    };

    GLuint _uniformsBuffer;
    GLuint _sphereBuffer;
    GLuint _planeBuffer;
    GLuint _lightBuffer;

    struct
    {
        GLint RayOrigin;
        GLint RayLookat;
        GLint Aspect;
    } Uniforms;

    GLuint _vao;

    struct Sphere
    {
        vec3 Center;
        float Radius;
        vec4 Color;
    };

    struct Plane
    {
        vec3 Normal;
        float D;
    };

    struct Light
    {
        vec3 Position;
        unsigned int : 32;
    };

    enum
    {
        MAX_RECURSION_DEPTH = 5,
        MAX_FB_WIDTH = 2048,
        MAX_FB_HEIGHT = 1024
    };

    enum DEBUG_MODE
    {
        DEBUG_NONE,
        DEBUG_REFLECTED,
        DEBUG_REFRACTED,
        DEBUG_REFLECTED_COLOR,
        DEBUG_REFRACTED_COLOR
    };

    GLuint _texComposite;
    GLuint _rayFBO[MAX_RECURSION_DEPTH];
    GLuint _texPosition[MAX_RECURSION_DEPTH];
    GLuint _texReflected[MAX_RECURSION_DEPTH];
    GLuint _texReflectionIntensity[MAX_RECURSION_DEPTH];
    GLuint _texRefracted[MAX_RECURSION_DEPTH];
    GLuint _texRefractionIntensity[MAX_RECURSION_DEPTH];

    int _maxDepth;
    int _debugDepth;
    DEBUG_MODE _debugMode;
    bool _paused;
};
}
