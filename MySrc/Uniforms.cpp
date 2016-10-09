#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>

#include "Utils.h"

namespace OpenGlSB
{
class Uniforms : public sb6::application
{
public:
    void startup()
    {
        _renderingProgram1 = GetShaderProgram("Shaders/Uniforms/Uniforms1.vert", "Shaders/Uniforms/Uniforms1.frag");
        _renderingProgram2 = GetShaderProgram("Shaders/Uniforms/Uniforms2.vert", "Shaders/Uniforms/Uniforms2.frag");
        glGenVertexArrays(1, &_vao1);
        glBindVertexArray(_vao1);

        glGenBuffers(1, &_vertsBuffer1);
        glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer1);
        const GLfloat verts1[] =
        {
            -0.35f, 0.25f, 0.5f, 1.0f,
            0.15f, -0.25f, 0.5f, 1.0f,
            -0.35f, -0.25f, 0.5f, 1.0f,
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts1), verts1, GL_STATIC_DRAW);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(5);

        glGenBuffers(1, &_vertsBufferV2);
        glBindBuffer(GL_ARRAY_BUFFER, _vertsBufferV2);
        const GLfloat colU[] =
        {
            1.0f, 1.0f, 0.0f, 1.0f,
            0.35f, 0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(colU), colU, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexBuffer(3, _vertsBufferV2, 0, 4 * sizeof(GLfloat));

        glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, 0);
        glVertexAttribBinding(1, 3);
        glEnableVertexAttribArray(1);


        glGenVertexArrays(1, &_vao2);
        glBindVertexArray(_vao2);
        glGenBuffers(1, &_vertsBuffer2);
        glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer2);

        const GLfloat verts2[] =
        {
            -0.15f, 0.25f, 0.5f, 1.0f,
            0.35f, 0.25f, 0.5f, 1.0f,
            0.35f, -0.25f, 0.5f, 1.0f
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts2), verts2, GL_STATIC_DRAW);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(5);

        const GLchar* uniformNames[4] =
        {
            "UniBlockStdv1.col",
            "UniBlockStdv1.colVec",
            "UniBlockStdv1.colArr",
            "UniBlockStdv1.colMat"
        };
        GLuint uniformIndices[4];
        glGetUniformIndices(_renderingProgram1, 4, uniformNames, uniformIndices);

        GLint offsets[4];
        GLint arrayStrides[4];
        GLint matrixStrides[4];
        glGetActiveUniformsiv(_renderingProgram1, 4, uniformIndices, GL_UNIFORM_OFFSET, offsets);
        glGetActiveUniformsiv(_renderingProgram1, 4, uniformIndices, GL_UNIFORM_ARRAY_STRIDE, arrayStrides);
        glGetActiveUniformsiv(_renderingProgram1, 4, uniformIndices, GL_UNIFORM_MATRIX_STRIDE, matrixStrides);
        char* buffer = (char*)malloc(4096);
        *((float*)(buffer + offsets[0])) = 1.0f;

        *((float*)(buffer + offsets[1])) = 1.0f;
        *((float*)(buffer + offsets[1] + sizeof(GLfloat))) = 0.0f;
        *((float*)(buffer + offsets[1] + 2 * sizeof(GLfloat))) = 0.0f;

        *((float*)(buffer + offsets[2])) = 0.0f;
        *((float*)(buffer + offsets[2] + arrayStrides[2])) = 1.0f;
        *((float*)(buffer + offsets[2] + 2 * arrayStrides[2])) = 0.0f;

        float mat[16] =
        {
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
        };

        int matOffset = offsets[3];
        int columnStride = matrixStrides[3];
        for (int i = 0; i < 4; i++)
        {
            GLuint offset = matOffset + columnStride * i;
            *((float*)(buffer + offset)) = mat[i * 4];
            *((float*)(buffer + offset + sizeof(GLfloat))) = mat[i * 4 + 1];
            *((float*)(buffer + offset + 2 * sizeof(GLfloat))) = mat[i * 4 + 2];
            *((float*)(buffer + offset + 3 * sizeof(GLfloat))) = mat[i * 4 + 3];
        }
        glGenBuffers(1, &_uniformBuffer1);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _uniformBuffer1); //bind _uniformBuffer1 to binding point 1 (index is not a block index!!!!)
        glBufferData(GL_UNIFORM_BUFFER, offsets[3] + 4 * matrixStrides[3] + 4 * sizeof(GLfloat), buffer, GL_STATIC_DRAW);

        free(buffer);
        buffer = (char*)malloc(4098);
        GLint currentOffset = 0;
        *(float*)buffer = 1.0f;
        currentOffset += 4 * sizeof(GLfloat);
        *(float*)(buffer + currentOffset) = 0.0f;
        *(float*)(buffer + currentOffset + sizeof(GLfloat)) = 0.0f;
        *(float*)(buffer + currentOffset + 2 * sizeof(GLfloat)) = 1.0f;
        *(float*)(buffer + currentOffset + 3 * sizeof(GLfloat)) = 1.0f;
        currentOffset += 4 * sizeof(GLfloat);
        *(float*)(buffer + currentOffset) = 0.0f;
        *(float*)(buffer + currentOffset + 4 * sizeof(GLfloat)) = 0.0f;
        *(float*)(buffer + currentOffset + 8 * sizeof(GLfloat)) = 0.7f;
        currentOffset += 12 * sizeof(GLfloat);
        GLuint stride = 4 * sizeof(GLfloat);

        float matstd140[9] =
        {
            0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f
        };
        for (int i = 0; i < 3; i++)
        {
            *(float*)(buffer + currentOffset) = matstd140[i * 3];
            *(float*)(buffer + currentOffset + sizeof(GLfloat)) = matstd140[i * 3 + 1];
            *(float*)(buffer + currentOffset + 2 * sizeof(GLfloat)) = matstd140[i * 3 + 2];
            currentOffset += stride;
        }
        glGenBuffers(1, &_uniformBufferstd140);
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, _uniformBufferstd140);
        glBufferData(GL_UNIFORM_BUFFER, currentOffset, buffer, GL_STATIC_DRAW);

        free(buffer);
    }

    void render(double currentTime)
    {
        //		GLint maj;
        //		GLint min;
        //		glGetIntegerv(GL_MAJOR_VERSION, &maj);
        //		glGetIntegerv(GL_MINOR_VERSION, &min);
        //		std::wstringstream wss;
        //		wss << maj << "|" << min << std::endl;
        //		OutputDebugString(wss.str().c_str());

        const GLfloat bckColor[] = {0.0f, 0.0f, 0.0f, 1.0f};

        glClearBufferfv(GL_COLOR, 0, bckColor);

        glBindVertexArray(_vao1);
        glUseProgram(_renderingProgram1);

        const GLfloat color[] = {1.0f, 1.0f, 0.0f, 1.0f};
        glUniform4fv(0, 1, color);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(_vao2);
        glUseProgram(_renderingProgram2);

        const GLfloat color2[] = {0.0f, 0.0f, 1.0f, 1.0f};
        glUniform4fv(0, 1, color2);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void shutdown()
    {
        glDeleteProgram(_renderingProgram1);
        glDeleteProgram(_renderingProgram2);

        glBindBuffer(GL_STATIC_DRAW, _vertsBuffer1);
        glDisableVertexAttribArray(5);
        glBindBuffer(GL_STATIC_DRAW, _vertsBuffer2);
        glDisableVertexAttribArray(5);

        glBindVertexArray(0);
        glDeleteVertexArrays(1, &_vao1);
        glDeleteVertexArrays(1, &_vao2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &_vertsBuffer1);
        glDeleteBuffers(1, &_vertsBuffer2);
        glDeleteBuffers(1, &_vertsBufferV2);
    }

private:
    GLuint _renderingProgram1;
    GLuint _renderingProgram2;
    GLuint _vao1;
    GLuint _vao2;
    GLuint _vertsBuffer1;
    GLuint _vertsBuffer2;
    GLuint _vertsBufferV2;
    GLuint _uniformBuffer1;
    GLuint _uniformBufferstd140;
};
}
