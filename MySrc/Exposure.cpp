#include <sb6.h>
#include <object.h>
#include <sb6ktx.h>

#include "vmath.h"
#include "ShaderProgram.h"

namespace OpenGlSB
{
using vmath::mat4;

class Exposure : public sb6::application
{
public:
    Exposure() : _exposure(1.0)
    {
    }

    void startup()
    {
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &_texture);
        sb6::ktx::file::load("media/textures/treelights_2k.ktx", _texture);
        glBindTexture(GL_TEXTURE_2D, _texture);

        _shader.CreateProgram();
        _shader.AttachShader("Shaders/Exposure/Exposure.vert");
        _shader.AttachShader("Shaders/Exposure/Exposure.frag");
        _shader.Link();


        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void render(double currentTime)
    {
        _shader.Use();

        static const GLfloat green[] = {0.0f, 0.25f, 0.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, green);

        glViewport(0, 0, info.windowWidth, info.windowHeight);
        glUniform1f(2, _exposure);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void shutdown()
    {
        _shader.DeleteProgram();
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &_vao);
    }

    void onKey(int key, int action) override
    {
        if (action)
        {
            switch (key)
            {
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
    ShaderProgram _shader;
    GLuint _vao;
    GLuint _elementsBuffer;
    GLuint _positionBuffer;
    GLuint _texture;
    float _exposure;
    bool _manyCubes;
    bool _wireframe;
    bool _paused;

    sb6::object _object;
};
}
//DECLARE_MAIN(Exposure);
