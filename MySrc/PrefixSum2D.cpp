#include <sb6.h>
#include <sb6ktx.h>

#include "Utils.h"
#include "vmath.h"
#include "ShaderProgram.h"

namespace OpenGlSB
{
#define NUM_ELEMENTS 2048

static float RandomFloat()
{
    static unsigned int Seed = 0x13371337;
    float res;
    unsigned int tmp;

    Seed *= 16807;
    tmp = Seed ^ (Seed >> 4) ^ (Seed << 15);
    *((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;
    return (res - 1.0f);
}

class PrefixSum2D : public sb6::application
{
public:
    void startup()
    {
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
        _compShader.CreateProgram();
        _compShader.AttachShader("Shaders/PrefixSum2D/PrefixSum2D.comp");
        _compShader.Link();

        _showShader.CreateProgram();
        _showShader.AttachShader("Shaders/PrefixSum2D/PrefixSum2D.vert");
        _showShader.AttachShader("Shaders/PrefixSum2D/PrefixSum2D.frag");
        _showShader.Link();

        glGenTextures(3, _images);
        _images[0] = sb6::ktx::file::load("media/textures/salad-gray.ktx");

        for (int i = 1; i < 3; i++)
        {
            glBindTexture(GL_TEXTURE_2D, _images[i]);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, NUM_ELEMENTS, NUM_ELEMENTS);
        }

        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void render(double currentTime)
    {
        _compShader.Use();

        glBindImageTexture(0, _images[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
        glBindImageTexture(1, _images[1], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        
        glDispatchCompute(NUM_ELEMENTS, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glBindImageTexture(0, _images[1], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
        glBindImageTexture(1, _images[2], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

        glDispatchCompute(NUM_ELEMENTS, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _images[2]);

        _showShader.Use();

        glViewport(0, 0, info.windowWidth, info.windowHeight);
        glBindVertexArray(_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void PrefixSumCalc(const float* input, float* output, int elements)
    {
        float f = 0.0f;
        int i;
        for (int i = 0; i < elements; i++)
        {
            f += input[i];
            output[i] = f;
        }
    }

private:
    ShaderProgram _compShader;
    ShaderProgram _showShader;
    GLuint _vao;
    GLuint _vertsBuffer;
    GLuint _elementsBuffer;
    GLuint _images[3];

    vmath::mat4 projMatrix;
    GLfloat mvLocation = 2;
    GLfloat projLocation = 3;
    GLuint _dataBuffer[2];
    float _inputData[NUM_ELEMENTS];
    float _outputData[NUM_ELEMENTS];
};
}
