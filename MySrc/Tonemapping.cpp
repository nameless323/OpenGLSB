#include <sb6.h>
#include <object.h>
#include <sb6ktx.h>

#include "vmath.h"
#include "ShaderProgram.h"

namespace OpenGlSB
{
using vmath::mat4;

class Tonemapping : public sb6::application
{
public:
    Tonemapping() : _exposure(1.0), _mode(0)
    {
    }

    void startup()
    {
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &_texture);
        sb6::ktx::file::load("media/textures/treelights_2k.ktx", _texture);
        glBindTexture(GL_TEXTURE_2D, _texture);

        _shaderNaive.CreateProgram();
        _shaderNaive.AttachShader("Shaders/Tonemapping/Tonemapping.vert");
        _shaderNaive.AttachShader("Shaders/Tonemapping/TonemappingNaive.frag");
        _shaderNaive.Link();

        _shaderAdaptive.CreateProgram();
        _shaderAdaptive.AttachShader("Shaders/Tonemapping/Tonemapping.vert");
        _shaderAdaptive.AttachShader("Shaders/Tonemapping/TonemappingAdaptive.frag");
        _shaderAdaptive.Link();

        _shaderExposure.CreateProgram();
        _shaderExposure.AttachShader("Shaders/Tonemapping/Tonemapping.vert");
        _shaderExposure.AttachShader("Shaders/Tonemapping/TonemappingExposure.frag");
        _shaderExposure.Link();
        _exposureLocation = glGetUniformLocation(_shaderExposure.GetHandler(), "exposure");


        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        //		static const GLfloat exposureLUT[20] = {11.0f, 6.0f, 3.2f, 2.8f, 2.2f, 1.90f, 1.80f, 1.80f, 1.70f, 1.70f, 1.60f, 1.60f, 1.50f, 1.50f, 1.40f, 1.40f, 1.30f, 1.20f, 1.10f, 1.00f};
        //		glGenTextures(1, &_textureLUT);
        //		glBindTexture(GL_TEXTURE_1D, _textureLUT);
        //		glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, 20);
        //		glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 20, GL_RED, GL_FLOAT, exposureLUT);
        //		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void render(double currentTime)
    {
        _shaderNaive.Use();

        static const GLfloat green[] = {0.0f, 0.25f, 0.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, green);

        glViewport(0, 0, info.windowWidth, info.windowHeight);

        //		glActiveTexture(GL_TEXTURE1);
        //		glBindTexture(GL_TEXTURE_1D, _textureLUT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture);

        switch (_mode)
        {
            case 0:
                _shaderNaive.Use();
                break;
            case 1:
                _shaderExposure.Use();
                glUniform1f(_exposureLocation, _exposure);
                break;
            case 2:
                _shaderAdaptive.Use();
                break;
        }

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void shutdown()
    {
        _shaderNaive.DeleteProgram();
        _shaderAdaptive.DeleteProgram();
        _shaderExposure.DeleteProgram();
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &_vao);
        glDeleteTextures(1, &_texture);
        glDeleteTextures(1, &_textureLUT);
    }

    void onKey(int key, int action) override
    {
        if (action)
        {
            switch (key)
            {
                case '1':
                case '2':
                case '3':
                    _mode = key - '1';
                    break;
                case 'M':
                    _mode = (_mode + 1) % 3;
                    break;
                case GLFW_KEY_KP_ADD:
                    _exposure *= 1.1f;
                    break;
                case GLFW_KEY_KP_SUBTRACT:
                    _exposure /= 1.1f;
                    break;
            }
        }
    }

private:
    ShaderProgram _shaderNaive;
    ShaderProgram _shaderAdaptive;
    ShaderProgram _shaderExposure;
    GLuint _vao;
    GLuint _elementsBuffer;
    GLuint _positionBuffer;
    GLuint _texture;
    GLuint _textureLUT;
    float _exposure;
    bool _manyCubes;
    bool _wireframe;
    bool _paused;
    GLint _exposureLocation;
    int _mode;

    sb6::object _object;
};
}
