#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"

class IndirectMultidrawSimple : public sb6::application
{
public:
	struct DrawArraysIndirectCommand
	{
		GLuint VertsCount;
		GLuint PrimitivesCount;
		GLuint FirstVertex;
		GLuint BaseVertex;
		GLuint BaseInstance;
	};
	void startup()
	{
		_renderingProgram = GetShaderProgram("Shaders/IndirectDraw/IndirectDrawSimple.vert", "Shaders/IndirectDraw/IndirectDrawSimple.frag");
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		const GLfloat verts[] =
		{
			-0.2f, -0.2, 0.5f, //triangle
			-0.2f, 0.2f, 0.5f,
			0.2f, -0.2f, 0.5f,

			-1.0f, -1.0f, 0.5f, //fullscreen quad
			-1.0f, 1.0f, 0.5f,
			1.0f, -1.0f, 0.5f,
			1.0f, 1.0f, 0.5f
		};
		const GLuint elems[] =
		{
			0, 1, 2,

			0, 1, 2,
			2, 1, 3
		};

		float angle = vmath::radians(180.0f);
		float c = cosf(angle);
		float s = sinf(angle);

		const GLfloat mats[] =
		{
			1.0f, 0.0f, 0.0f, 0.0f, //trig 1
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			-0.2f, -0.2f, 0.0f, 1.0f,

			c, s , 0.0f, 0.0f, //trig 2
			-s, c, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.2f, 0.2f, 0.0f, 1.0f,

			0.1f, 0.0f, 0.0f, 0.0f, //cube 1
			0.0f, 0.1f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.1f, 0.0f,
			-0.8f, -0.8f, 0.0f, 1.0f,

			0.1f, 0.0f, 0.0f, 0.0f, //cube 2
			0.0f, 0.1f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.1f, 0.0f,
			-0.8f, 0.8f, 0.0f, 1.0f,

			0.1f, 0.0f, 0.0f, 0.0f, //cube 3
			0.0f, 0.1f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.1f, 0.0f,
			0.8f, 0.8f, 0.0f, 1.0f,

			0.1f, 0.0f, 0.0f, 0.0f, //cube 4
			0.0f, 0.1f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.1f, 0.0f,
			0.8f, -0.8f, 0.0f, 1.0f
		};
		const GLfloat colors[] =
		{
			1.0f, 0.0f, 0.0f, //trig 1-2
			0.0f, 1.0f, 0.0f,

			0.0f, 0.0f, 1.0f, //quad 1-4
			1.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 1.0f, 
			1.0f, 1.0f, 1.0f
		};

		glGenBuffers(5, _buffers);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _buffers[0]);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, 2 * sizeof(DrawArraysIndirectCommand), nullptr, GL_STATIC_DRAW);

		DrawArraysIndirectCommand* commandBuffer = (DrawArraysIndirectCommand*)glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, 2 * sizeof(DrawArraysIndirectCommand), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		commandBuffer[0].VertsCount = 3;
		commandBuffer[0].PrimitivesCount = 2;
		commandBuffer[0].FirstVertex = 0;
		commandBuffer[0].BaseVertex = 0;
		commandBuffer[0].BaseInstance = 0;

		commandBuffer[1].VertsCount = 6;
		commandBuffer[1].PrimitivesCount = 4;
		commandBuffer[1].FirstVertex = 3;
		commandBuffer[1].BaseVertex = 3;
		commandBuffer[1].BaseInstance = 2;

		glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);

		glBindBuffer(GL_ARRAY_BUFFER, _buffers[1]); //vertices
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		_modelLocation = glGetAttribLocation(_renderingProgram, "model");
		int sizeOfGlFloat = sizeof(GLfloat);
		int numOfElems = 16;
		int stride = numOfElems * sizeOfGlFloat;
		glBindBuffer(GL_ARRAY_BUFFER, _buffers[2]); //matrices
		glBufferData(GL_ARRAY_BUFFER, sizeof(mats), mats, GL_STATIC_DRAW);
		glVertexAttribPointer(_modelLocation, 4, GL_FLOAT, GL_FALSE, stride, 0);
		glVertexAttribPointer(_modelLocation + 1, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(4 * sizeOfGlFloat));
		glVertexAttribPointer(_modelLocation + 2, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(8 * sizeOfGlFloat));
		glVertexAttribPointer(_modelLocation + 3, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(12 * sizeOfGlFloat));

		glEnableVertexAttribArray(_modelLocation);
		glEnableVertexAttribArray(_modelLocation + 1);
		glEnableVertexAttribArray(_modelLocation + 2);
		glEnableVertexAttribArray(_modelLocation + 3);

		glVertexAttribDivisor(_modelLocation, 1);
		glVertexAttribDivisor(_modelLocation + 1, 1);
		glVertexAttribDivisor(_modelLocation + 2, 1);
		glVertexAttribDivisor(_modelLocation + 3, 1);

		glBindBuffer(GL_ARRAY_BUFFER, _buffers[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(2);
		glVertexAttribDivisor(2, 1);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers[4]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);
	}

	void onResize(int w, int h)
	{
		application::onResize(w, h);
	}

	void render(double currentTime)
	{
		glUseProgram(_renderingProgram);
		glBindVertexArray(_vao);
		const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, bckColor);
		glClearBufferfv(GL_DEPTH, 0, white);
		//		glClear(GL_DEPTH_BUFFER_BIT);
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, 2, 0);
		//glMultiDrawArraysIndirect(GL_TRIANGLES, nullptr, 2, 0);
		glBindVertexArray(0);
	}

	void shutdown()
	{
		glDeleteProgram(_renderingProgram);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(_modelLocation);
		glDisableVertexAttribArray(_modelLocation + 1);
		glDisableVertexAttribArray(_modelLocation + 2);
		glDisableVertexAttribArray(_modelLocation + 3);

		glBindVertexArray(0);
		glDeleteVertexArrays(1, &_vao);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(5, _buffers);
	}

private:
	GLuint _renderingProgram;
	GLuint _modelLocation;
	GLuint _vao;
	GLuint _buffers[4]; //0 - command, 1 - position, 2 - mat, 3 - colors
	

	vmath::mat4 projMatrix;
	GLfloat mvLocation = 2;
	GLfloat projLocation = 3;

};

//DECLARE_MAIN(IndirectMultidrawSimple);

