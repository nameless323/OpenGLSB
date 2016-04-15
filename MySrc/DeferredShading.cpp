#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"
#include "ShaderProgram.h"
#include <object.h>

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
    {}

    void startup()
    {
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
        
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void render(double currentTime)
    {
    }

    void shutdown()
    {
        glDeleteTextures(3, _GBufferTex);
        glDeleteFramebuffers(1, &_GBuffer);
        _renderingShader.DeleteProgram();
        _lightShader.DeleteProgram();
        _nmShader.DeleteProgram();
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

    virtual void onKey(int key, int action)
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

//DECLARE_MAIN(DeferredShading);

