//
// Create shadows using shadow map
//

#include <sb6.h>
#include <cmath>
#include <object.h>

#include "Utils.h"
#include "vmath.h"
#include "ShaderProgram.h"

#define DEPTH_TEXTURE_SIZE 4096
#define FRUSTUM_DEPTH 1000

namespace OpenGlSB
{
class ShadowMapping : public sb6::application
{
public:
    ShadowMapping() : _paused(false), Mode(RENDER_FULL)
    {
    }

    void LoadShaders()
    {
        _lightShader.CreateProgram();
        _lightShader.AttachShader("Shaders/ShadowMapping/Light.vert");
        _lightShader.AttachShader("Shaders/ShadowMapping/Light.frag");
        _lightShader.Link();
        Uniforms.Light.MVP = glGetUniformLocation(_lightShader.GetHandler(), "mvp");

        _cameraShader.CreateProgram();
        _cameraShader.AttachShader("Shaders/ShadowMapping/Camera.vert");
        _cameraShader.AttachShader("Shaders/ShadowMapping/Camera.frag");
        _cameraShader.Link();
        Uniforms.View.Proj = glGetUniformLocation(_cameraShader.GetHandler(), "proj_matrix");
        Uniforms.View.MV = glGetUniformLocation(_cameraShader.GetHandler(), "mv_matrix");
        Uniforms.View.ShadowMatrix = glGetUniformLocation(_cameraShader.GetHandler(), "shadow_matrix");
        Uniforms.View.FullShading = glGetUniformLocation(_cameraShader.GetHandler(), "full_shading");


        _viewShader.CreateProgram();
        _viewShader.AttachShader("Shaders/ShadowMapping/View.vert");
        _viewShader.AttachShader("Shaders/ShadowMapping/View.frag");
        _viewShader.Link();
    }

    void startup() override
    {
        LoadShaders();
        static const char* const objNames[] =
            {
                "media/objects/dragon.sbm",
                "media/objects/sphere.sbm",
                "media/objects/cube.sbm",
                "media/objects/torus.sbm"
            };
        for (int i = 0; i < OBJECT_COUNT; i++)
        {
            Objects[i].Obj.load(objNames[i]);
        }
        glGenFramebuffers(1, &_depthFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, _depthFBO);

        glGenTextures(1, &_depthTex);
        glBindTexture(GL_TEXTURE_2D, _depthTex);

        glTexStorage2D(GL_TEXTURE_2D, 11, GL_DEPTH_COMPONENT32F, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthTex, 0);

        glGenTextures(1, &_depthDebugTex);
        glBindTexture(GL_TEXTURE_2D, _depthDebugTex);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _depthDebugTex, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_DEPTH_TEST);
        glGenVertexArrays(1, &_quadVao);
        glBindVertexArray(_quadVao);
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

        const float f = (float)totalTime + 30.0f;
        vmath::vec3 lightPosition = vmath::vec3(20.0f, 20.0f, 20.0f);
        vmath::vec3 viewPosition = vmath::vec3(0.0f, 0.0f, 40.0f);

        _lightProjMatrix = vmath::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 200.0f);
        _lightViewMatrix = vmath::lookat(lightPosition,
                                         vmath::vec3(0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));

        _cameraProjMatrix = vmath::perspective(50.0f,
                                               (float)info.windowWidth / (float)info.windowHeight,
                                               1.0f,
                                               200.0f);

        _cameraViewMatrix = vmath::lookat(viewPosition,
                                          vmath::vec3(0.0f),
                                          vmath::vec3(0.0f, 1.0f, 0.0f));

        Objects[0].ModelMatrix = vmath::rotate(f * 14.5f, 0.0f, 1.0f, 0.0f) *
            vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f) *
            vmath::translate(0.0f, -4.0f, 0.0f);

        Objects[1].ModelMatrix = vmath::rotate(f * 3.7f, 0.0f, 1.0f, 0.0f) *
            vmath::translate(sinf(f * 0.37f) * 12.0f, cosf(f * 0.37f) * 12.0f, 0.0f) *
            vmath::scale(2.0f);

        Objects[2].ModelMatrix = vmath::rotate(f * 6.45f, 0.0f, 1.0f, 0.0f) *
            vmath::translate(sinf(f * 0.25f) * 10.0f, cosf(f * 0.25f) * 10.0f, 0.0f) *
            vmath::rotate(f * 99.0f, 0.0f, 0.0f, 1.0f) *
            vmath::scale(2.0f);

        Objects[3].ModelMatrix = vmath::rotate(f * 5.25f, 0.0f, 1.0f, 0.0f) *
            vmath::translate(sinf(f * 0.51f) * 14.0f, cosf(f * 0.51f) * 14.0f, 0.0f) *
            vmath::rotate(f * 120.3f, 0.707106f, 0.0f, 0.707106f) *
            vmath::scale(2.0f);

        glEnable(GL_DEPTH_TEST);
        renderScene(totalTime, true);

        if (Mode == RENDER_DEPTH)
        {
            glDisable(GL_DEPTH_TEST);
            glBindVertexArray(_quadVao);
            _viewShader.Use();
            glBindTexture(GL_TEXTURE_2D, _depthDebugTex);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        else
        {
            renderScene(totalTime, false);
        }
    }

    void renderScene(double currentTime, bool fromLight)
    {
        static const GLfloat ones[] = {1.0f};
        static const GLfloat zero[] = {0.0f};
        static const GLfloat gray[] = {0.1f, 0.1f, 0.1f, 0.0f};
        static const vmath::mat4 scaleBiasMatrix = vmath::mat4(vmath::vec4(0.5f, 0.0f, 0.0f, 0.0f),
                                                               vmath::vec4(0.0f, 0.5f, 0.0f, 0.0f),
                                                               vmath::vec4(0.0f, 0.0f, 0.5f, 0.0f),
                                                               vmath::vec4(0.5f, 0.5f, 0.5f, 1.0f));
        vmath::mat4 lightVPmatrix = _lightProjMatrix * _lightViewMatrix;
        vmath::mat4 shadowSBPVMatrix = scaleBiasMatrix * _lightProjMatrix * _lightViewMatrix;

        if (fromLight)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, _depthFBO);
            glViewport(0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(4.0f, 4.0f);
            _lightShader.Use();
            static const GLenum buffs[] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, buffs);
            glClearBufferfv(GL_COLOR, 0, zero);
        }
        else
        {
            glViewport(0, 0, info.windowWidth, info.windowHeight);
            glClearBufferfv(GL_COLOR, 0, gray);
            _cameraShader.Use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _depthTex);
            glUniformMatrix4fv(Uniforms.View.Proj, 1, GL_FALSE, _cameraProjMatrix);
            glDrawBuffer(GL_BACK);
        }
        glClearBufferfv(GL_DEPTH, 0, ones);
        for (int i = 0; i < 4; i++)
        {
            if (fromLight)
            {
                glUniformMatrix4fv(Uniforms.Light.MVP, 1, GL_FALSE, lightVPmatrix * Objects[i].ModelMatrix);
            }
            else
            {
                vmath::mat4 shadowMatrix = shadowSBPVMatrix * Objects[i].ModelMatrix;
                glUniformMatrix4fv(Uniforms.View.ShadowMatrix, 1, GL_FALSE, shadowMatrix);
                glUniformMatrix4fv(Uniforms.View.MV, 1, GL_FALSE, _cameraViewMatrix*Objects[i].ModelMatrix);
                glUniform1i(Uniforms.View.FullShading, Mode == RENDER_FULL ? 1 : 0);
            }
            Objects[i].Obj.render();
        }
        if (fromLight)
        {
            glDisable(GL_POLYGON_OFFSET_FILL);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void onKey(int key, int action)
    {
        if (action)
        {
            switch (key)
            {
            case '1':
                Mode = RENDER_FULL;
                break;
            case '2':
                Mode = RENDER_LIGHT;
                break;
            case '3':
                Mode = RENDER_DEPTH;
                break;
            case 'P':
                _paused = !_paused;
                break;
            }
        }
    }

private:
    ShaderProgram _lightShader;
    ShaderProgram _cameraShader;
    ShaderProgram _viewShader;

    struct
    {
        struct
        {
            GLint MVP;
        } Light;

        struct
        {
            GLint MV;
            GLint Proj;
            GLint ShadowMatrix;
            GLint FullShading;
        } View;
    } Uniforms;

    GLuint _depthFBO;
    GLuint _depthTex;
    GLuint _depthDebugTex;

    enum
    {
        OBJECT_COUNT = 4
    };

    struct
    {
        sb6::object Obj;
        vmath::mat4 ModelMatrix;
    } Objects[OBJECT_COUNT];

    vmath::mat4 _lightViewMatrix;
    vmath::mat4 _lightProjMatrix;

    vmath::mat4 _cameraViewMatrix;
    vmath::mat4 _cameraProjMatrix;

    GLuint _quadVao;

    enum
    {
        RENDER_FULL,
        RENDER_LIGHT,
        RENDER_DEPTH
    } Mode;

    bool _paused;
};
}
