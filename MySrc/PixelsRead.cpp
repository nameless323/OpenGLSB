//
// Using read pixels to make screenshot.
//

#include <sb6.h>
#include <object.h>

#include "vmath.h"
#include "ShaderProgram.h"

namespace OpenGlSB
{
using vmath::mat4;

class PixelsRead : public sb6::application
{
public:
    PixelsRead() : Mode(MODE_READ_OFF), _paused(false)
    {}

    void startup()
    {
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
        _shader.CreateProgram();
        _shader.AttachShader("Shaders/DepthClamp/DepthClamp.vert");
        _shader.AttachShader("Shaders/DepthClamp/DepthClamp.frag");
        _shader.Link();

        _mvLocation = glGetUniformLocation(_shader.GetHandler(), "mv_matrix");
        _projLocation = glGetUniformLocation(_shader.GetHandler(), "proj_matrix");

        _object.load("media/objects/dragon.sbm");

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glGenBuffers(5, _pixelBuffers);
        for (int i = 0; i < 5; i++)
        {
            glBindBuffer(GL_PIXEL_PACK_BUFFER, _pixelBuffers[i]);
            glBufferData(GL_PIXEL_PACK_BUFFER, 30 * 30 * 4, nullptr, GL_STREAM_COPY);
        }
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

        _shader.Use();
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void onKey(int key, int action)
    {
        if (!action)
            return;

        switch (key)
        {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            Mode = (MODE)(key - '1');
            break;
        case 'P':
            _paused = !_paused;
            break;
        };
    }

    void render(double currentTime)
    {
        glViewport(0, 0, info.windowWidth, info.windowHeight);
        float f = (float)currentTime;
        unsigned char pixel[4 * 30 * 30];
        static unsigned int frame_index = 0;

#if _MSC_VER
        if (_paused)
            Sleep(200);
#else
        if (_paused)
            usleep(200000);
#endif

        static const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        static const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, bckColor);
        glClearBufferfv(GL_DEPTH, 0, white);
        _shader.Use();

        vmath::mat4 proj = vmath::perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight, 1.8f, 1000.0f);
        glUniformMatrix4fv(_projLocation, 1, GL_FALSE, proj);

        mat4 mv = vmath::translate(0.0f, -5.0f, -100.0f) *
            vmath::rotate(f * 5.0f, 0.0f, 1.0f, 0.0f);
        glUniformMatrix4fv(_mvLocation, 1, GL_FALSE, mv);

        _object.render(8 * 8);

        switch(Mode)
        {
        case MODE_READ_OFF:
            break;
        case MODE_READ_TO_SYSMEM:
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
            glReadPixels(info.windowWidth / 2,
                info.windowHeight / 2,
                30, 30,
                GL_RGBA, GL_UNSIGNED_BYTE,
                pixel);
            break;
            case MODE_READ_PBO_FRAME4:
            glBindBuffer(GL_PIXEL_PACK_BUFFER, _pixelBuffers[frame_index % 5]);
            glReadPixels(info.windowWidth / 2,
                info.windowHeight / 2,
                30, 30,
                GL_RGBA, GL_UNSIGNED_BYTE,
                nullptr);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, _pixelBuffers[(frame_index - (Mode - MODE_READ_PBO_SAME_FRAME)) % 5]);
            glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, 30 * 30 * 4, GL_MAP_READ_BIT);
            glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
            break;
        }

        frame_index++;
    }

    void shutdown()
    {
        _shader.DeleteProgram();
        _object.free();
    }

private:
    ShaderProgram _shader;
    GLuint _vao;
    GLuint _vertsBuffer;
    GLuint _elementsBuffer;
    GLuint _uniformBuffer;

    mat4 _projMatrix;
    GLint _mvLocation = 2;
    GLint _mvpLocation = 3;
    GLint _explodeFactor;
    GLint _projLocation;
    GLint _normalLengthLocation;
    GLint _viewpointLocation;

    sb6::object _object;

    bool _paused;
    GLuint _pixelBuffers[5];
    enum MODE
    {
        MODE_READ_OFF,
        MODE_READ_TO_SYSMEM,
        MODE_READ_PBO_SAME_FRAME,
        MODE_READ_PBO_FRAME1,
        MODE_READ_PBO_FRAME2,
        MODE_READ_PBO_FRAME3,
        MODE_READ_PBO_FRAME4
    } Mode;
};
}
