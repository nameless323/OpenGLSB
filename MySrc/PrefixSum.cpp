//
// Prefix sum computation using compute shaders.
//

#include <sb6.h>

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

class PrefixSum : public sb6::application
{
public:
    void startup()
    {
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
        _shader.CreateProgram();
        _shader.AttachShader("Shaders/PrefixSum/PrefixSum.comp");
        _shader.Link();
        glShaderStorageBlockBinding(_shader.GetHandler(), 0, 0);
        glShaderStorageBlockBinding(_shader.GetHandler(), 1, 1);
        
        glGenBuffers(2, _dataBuffer);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, _dataBuffer[0]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_ELEMENTS * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, _dataBuffer[1]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_ELEMENTS * sizeof(float), nullptr, GL_DYNAMIC_COPY);

        for (int i = 0; i < NUM_ELEMENTS; i++)
        {
            _inputData[i] = RandomFloat();
        }
        PrefixSumCalc(_inputData, _outputData, NUM_ELEMENTS);
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void render(double currentTime)
    {
        float* ptr;
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, _dataBuffer[0], 0, sizeof(float) * NUM_ELEMENTS);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * NUM_ELEMENTS, _inputData);
        
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, _dataBuffer[1], 0, sizeof(float) * NUM_ELEMENTS);

        _shader.Use();
        glDispatchCompute(1, 1, 1);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glFinish();

        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, _dataBuffer[1], 0, sizeof(float) * NUM_ELEMENTS);
        ptr = (float*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * NUM_ELEMENTS, GL_MAP_READ_BIT);
        char buffer[1024];
        sprintf(buffer, "%2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f "
            "%2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f\n",
            ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6], ptr[7],
            ptr[8], ptr[9], ptr[10], ptr[11], ptr[12], ptr[13], ptr[14], ptr[15]);
        OutputDebugStringA(buffer);
        sprintf(buffer, "%2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f "
            "%2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f %2.2f\n",
            _outputData[0], _outputData[1], _outputData[2], _outputData[3], _outputData[4], _outputData[5], _outputData[6], _outputData[7],
            _outputData[8], _outputData[9], _outputData[10], _outputData[11], _outputData[12], _outputData[13], _outputData[14], _outputData[15]);
        OutputDebugStringA(buffer);
        OutputDebugStringA("---\n");
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
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
    ShaderProgram _shader;
    GLuint _vao;
    GLuint _vertsBuffer;
    GLuint _elementsBuffer;

    vmath::mat4 projMatrix;
    GLfloat mvLocation = 2;
    GLfloat projLocation = 3;
    GLuint _dataBuffer[2];
    float _inputData[NUM_ELEMENTS];
    float _outputData[NUM_ELEMENTS];
};
}
