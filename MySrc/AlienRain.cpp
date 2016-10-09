//
// Using texture array to texture fallen sprites.
//

#include <sb6.h>
#include <sb6ktx.h>
#include <cmath>
#include <sstream>

#include "Utils.h"
#include "vmath.h"

namespace OpenGlSB
{
static unsigned int seed = 0x13371337;

static float random_float()
{
    float res;
    unsigned int tmp;

    seed *= 16807;

    tmp = seed ^ (seed >> 4) ^ (seed << 15);

    *((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

    return (res - 1.0f);
}

class AlienRain : public sb6::application
{
public:
    void startup()
    {
        _renderingProgram = GetShaderProgram("Shaders/AlienRain/AlienRain.vert", "Shaders/AlienRain/AlienRain.frag");
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        //		glEnable(GL_DEPTH_TEST);
        //		glDepthFunc(GL_LEQUAL);

        _alienTex = sb6::ktx::file::load("media/textures/aliens.ktx");
        GLuint textureUnit = 0;
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, _alienTex);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glGenBuffers(1, &_rainBuffer);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _rainBuffer);
        glBufferData(GL_UNIFORM_BUFFER, 256 * sizeof(vmath::vec4), NULL, GL_DYNAMIC_DRAW);

        for (int i = 0; i < 256; i++)
        {
            dropletXOffset[i] = random_float() * 2.0f - 1.0f;
            dropletRotSpeed[i] = (random_float() + 0.5f) * ((i & 1) ? -3.0f : 3.0f);
            dropletFallSpeed[i] = random_float() + 0.2f;
        }

        glBindVertexArray(_vao); //?
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void render(double currentTime)
    {
        static const GLfloat black[] = {0.0f, 0.0f, 0.0f, 0.0f};
        glViewport(0, 0, info.windowWidth, info.windowHeight);
        float t = static_cast<float>(currentTime);

        glClearBufferfv(GL_COLOR, 0, black);
        glUseProgram(_renderingProgram);
        //glBindBufferBase(GL_UNIFORM_BUFFER, 0, _rainBuffer);
        vmath::vec4* droplet = (vmath::vec4*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 256 * sizeof(vmath::vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        for (int i = 0; i < 256; i++)
        {
            droplet[i][0] = dropletXOffset[i];
            droplet[i][1] = 2.0f - fmodf((t + float(i)) * dropletFallSpeed[i], 4.31f);
            droplet[i][2] = t * dropletRotSpeed[i];
            droplet[i][3] = 0.0f;
        }
        glUnmapBuffer(GL_UNIFORM_BUFFER);

        int alienIndex;
        for (alienIndex = 0; alienIndex < 256; alienIndex++)
        {
            glVertexAttribI1i(0, alienIndex);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }

    void shutdown()
    {
        glDeleteProgram(_renderingProgram);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(4);

        glBindVertexArray(0);
        glDeleteVertexArrays(1, &_vao);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &_rainBuffer);
        glDeleteBuffers(1, &_uvBuffer);
    }

    void GenerateTexture(float* data, int width, int height)
    {
        int x, y;

        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                data[(y * width + x) * 4 + 0] = (float)((x & y) & 0xFF) / 255.0f;
                data[(y * width + x) * 4 + 1] = (float)((x | y) & 0xFF) / 255.0f;
                data[(y * width + x) * 4 + 2] = (float)((x ^ y) & 0xFF) / 255.0f;
                data[(y * width + x) * 4 + 3] = 1.0f;
            }
        }
    }

private:
    GLuint _renderingProgram;
    GLuint _vao;
    GLuint _rainBuffer;
    GLuint _uvBuffer;
    GLuint _sampler;
    GLuint _alienTex;

    float dropletXOffset[256];
    float dropletRotSpeed[256];
    float dropletFallSpeed[256];

    vmath::mat4 projMatrix;
    GLfloat mvLocation = 2;
    GLfloat projLocation = 3;
};
}
