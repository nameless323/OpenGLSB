//
// Example of gloss map usage.
//

#include <sb6.h>
#include <sb6ktx.h>
#include <object.h>

#include "Utils.h"
#include "vmath.h"
#include "ShaderProgram.h"

namespace OpenGlSB
{
class Glossmap : public sb6::application
{
public:
    Glossmap()
    {
    }

    void LoadShaders()
    {
        _shader.CreateProgram();
        _shader.AttachShader("Shaders/Glossmap/Glossmap.vert");
        _shader.AttachShader("Shaders/Glossmap/Glossmap.frag");
        _shader.Link();

        Uniforms.MV = glGetUniformLocation(_shader.GetHandler(), "mv_matrix");
        Uniforms.Proj = glGetUniformLocation(_shader.GetHandler(), "proj_matrix");
    }

    void startup() override
    {
        LoadShaders();
        glActiveTexture(GL_TEXTURE0);
        _texEnvMap = sb6::ktx::file::load("media/textures/envmaps/mountains3d.ktx");
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glActiveTexture(GL_TEXTURE1);
        _texGlossmap = sb6::ktx::file::load("media/textures/pattern1.ktx");

        _object.load("media/objects/torus_nrms_tc.sbm");
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void render(double currentTime) override
    {
        float t = (float)currentTime;
        static const float black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        static const float green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
        static const float one = 1.0f;
        static double lastTime = 0.0;
        static double totalTime = 0.0;


        glViewport(0, 0, info.windowWidth, info.windowHeight);
        glClearBufferfv(GL_COLOR, 0, black);
        glClearBufferfv(GL_DEPTH, 0, &one);

        vmath::mat4 proj = vmath::perspective(60.0f,
            (float)info.windowWidth / (float)info.windowHeight,
            0.1f,
            1000.0f);
        vmath::mat4 mv = vmath::translate(0.0f, 0.0f, -3.0f) *
            vmath::rotate((float)currentTime * 13.75f, 0.0f, 1.0f, 0.0f) *
            vmath::rotate((float)currentTime * 7.75f, 0.0f, 0.0f, 1.0f) *
            vmath::rotate((float)currentTime * 15.3f, 1.0f, 0.0f, 0.0f);

        _shader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, _texEnvMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _texGlossmap);

        glUniformMatrix4fv(Uniforms.Proj, 1, GL_FALSE, proj);
        glUniformMatrix4fv(Uniforms.MV, 1, GL_FALSE, mv);
        glEnable(GL_DEPTH_TEST);


        _object.render();

    }

    void onKey(int key, int action)
    {
        if (action)
        {
           
        }
    }

private:
    ShaderProgram _shader;

    struct
    {
        GLuint MV;
        GLuint Proj;
    } Uniforms;

    GLuint _texEnvMap;
    GLuint _texGlossmap;
    sb6::object _object;
};
}
