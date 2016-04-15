#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"
#include "ShaderProgram.h"
#include <object.h>

class Cellshading : public sb6::application
{
public:
    void startup()
    {
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
        _renderingProgram.CreateProgram();
        _renderingProgram.AttachShader("Shaders/Cellshading/Cellshading.vert");
        _renderingProgram.AttachShader("Shaders/Cellshading/Cellshading.frag");
        _renderingProgram.Link();
        projLocation = glGetUniformLocation(_renderingProgram.GetHandler(), "proj_matrix");
        mvLocation = glGetUniformLocation(_renderingProgram.GetHandler(), "mv_matrix");

        static const GLubyte tex[] =
        {
            0x44, 0x00, 0x00, 0x00,
            0x88, 0x00, 0x00, 0x00,
            0xCC, 0x00, 0x00, 0x00,
            0xFF, 0x00, 0x00, 0x00
        };

        glGenTextures(1, &_texToon);
        glBindTexture(GL_TEXTURE_1D, _texToon);
        glTexStorage1D(GL_TEXTURE_1D, 1, GL_RGB8, sizeof(tex) / 4);
        glTexSubImage1D(GL_TEXTURE_1D, 0, 0, sizeof(tex) / 4, GL_RGBA, GL_UNSIGNED_BYTE, tex);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        
        _object.load("media/objects/torus_nrms_tc.sbm");

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        _renderingProgram.Use();
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void render(double currentTime)
    {

        const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, bckColor);
        glClearBufferfv(GL_DEPTH, 0, white);
        glBindTexture(GL_TEXTURE_1D, _texToon);
        glViewport(0, 0, info.windowWidth, info.windowHeight);

        vmath::mat4 proj = vmath::perspective(60.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);
        vmath::mat4 mv = vmath::translate(0.0f, 0.0f, -3.0f) *
            vmath::rotate((float)currentTime * 13.75f, 0.0f, 1.0f, 0.0f) *
            vmath::rotate((float)currentTime * 7.75f, 0.0f, 0.0f, 1.0f) *
            vmath::rotate((float)currentTime * 15.3f, 1.0f, 0.0f, 0.0f);

        glUniformMatrix4fv(mvLocation, 1, GL_FALSE, mv);
        glUniformMatrix4fv(projLocation, 1, GL_FALSE, proj);
        _object.render();
    }

    void shutdown()
    {
        _object.free();
    }

private:
    ShaderProgram _renderingProgram;
    GLuint _texToon;

    vmath::mat4 projMatrix;
    GLfloat mvLocation = 2;
    GLfloat projLocation = 3;
    sb6::object _object;
};

//DECLARE_MAIN(Cellshading);

