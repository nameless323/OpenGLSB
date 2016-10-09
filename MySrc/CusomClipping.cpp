#include <sb6.h>
#include <cmath>
#include <sstream>
#include <object.h>

#include "Utils.h"
#include "vmath.h"

namespace OpenGlSB
{
using vmath::mat4;
using vmath::vec4;

class CustomClipping : public sb6::application
{
public:
    CustomClipping() : _shader(0), _paused(false)
    {
    }

    void startup()
    {
        _object.load("media/objects/dragon.sbm");
        LoadShaders();
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void render(double currentTime)
    {
        static const GLfloat bckColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
        static const GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};

        static double lastTime = 0.0f;
        static double totalTime = 0.0f;

        if (!_paused)
            totalTime += (currentTime - lastTime);
        lastTime = currentTime;
        float f = (float)totalTime;

        glClearBufferfv(GL_COLOR, 0, bckColor);
        glClearBufferfv(GL_DEPTH, 0, white);

        glUseProgram(_shader);

        mat4 proj = vmath::perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);
        mat4 mv = vmath::translate(0.0f, 0.0f, -15.0f) * vmath::rotate(f * 0.34f, 0.0f, 1.0f, 0.0f) * vmath::translate(0.0f, -4.0f, 0.0f);
        mat4 planeMat = vmath::rotate(f * 6.0f, 1.0f, 0.0f, 0.0f) * vmath::rotate(f * 7.3f, 0.0f, 1.0f, 0.0f);
        vec4 plane = planeMat[0];
        plane[3] = 0.0f;
        plane = normalize(plane);

        vec4 clipSphere = vec4(sinf(f * 0.7f) * 3.0f, cosf(f * 1.9f) * 3.0f, sinf(f * 0.1f) * 3.0f, cosf(f * 1.7f) + 2.5f);
        glUniformMatrix4fv(Uniforms.ProjMat, 1, GL_FALSE, proj);
        glUniformMatrix4fv(Uniforms.MV, 1, GL_FALSE, mv);
        glUniform4fv(Uniforms.ClipPlane, 1, plane);
        glUniform4fv(Uniforms.ClipSphere, 1, clipSphere);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CLIP_DISTANCE0);
        glEnable(GL_CLIP_DISTANCE1);

        _object.render();
    }

    void shutdown()
    {
        glDeleteProgram(_shader);
    }

private:
    GLuint _shader;
    sb6::object _object;
    bool _paused;

    struct
    {
        GLuint ProjMat;
        GLuint MV;
        GLuint ClipPlane;
        GLuint ClipSphere;
    } Uniforms;

    void LoadShaders()
    {
        if (_shader)
        glDeleteProgram(_shader);
        _shader = GetShaderProgram("Shaders/CustomClipping/CustomClipping.vert", "Shaders/CustomClipping/CustomClipping.frag");

        Uniforms.ProjMat = glGetUniformLocation(_shader, "proj");
        Uniforms.MV = glGetUniformLocation(_shader, "mv");
        Uniforms.ClipPlane = glGetUniformLocation(_shader, "clipPlane");
        Uniforms.ClipSphere = glGetUniformLocation(_shader, "clipSphere");
    }
};
}
//DECLARE_MAIN(CustomClipping);
