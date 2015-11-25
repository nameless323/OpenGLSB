#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"

class InstancingDivisor : public sb6::application
{
public:
	void startup()
	{
		_renderingProgram = GetShaderProgram("Shaders/InstancingDivisor/Divisor.vert", "Shaders/InstancingDivisor/Divisor.frag");
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glGenBuffers(1, &_vertsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer);
		const GLfloat verts[] =
		{
			-0.5f, -0.5f, 0.5f, 1.0f,
			-0.5f, 0.5f, 0.5f, 1.0f,
			0.5f, 0.5f, 0.5f, 1.0f,
			0.5f, -0.5f, 0.5f, 1.0f
		};
		const GLfloat instancedColors[] =
		{
			1.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 0.0f, 1.0f
		};
		const GLfloat instancedOffsets[] =
		{
			-2.0f, 2.0f, 0.0f, 0.0f,
			-2.0f, -2.0f, 0.0f, 0.0f,
			2.0f, 2.0f, 0.0f, 0.0f,
			2.0f, -2.0f, 0.0f, 0.0f
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(verts) + sizeof(instancedColors) + sizeof(instancedOffsets), nullptr, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(verts), sizeof(instancedColors), instancedColors);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(verts) + sizeof(instancedColors), sizeof(instancedOffsets), instancedOffsets);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)sizeof(verts));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(verts) + sizeof(instancedColors)));
		glEnableVertexAttribArray(2);

		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);

		const GLubyte indices[] =
		{
			0, 1, 2,
			0, 2, 3
		};
		glGenBuffers(1, &_elementsBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementsBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glUseProgram(_renderingProgram);
		_mvpLocation = glGetUniformLocation(_renderingProgram, "mvp");
		glBindVertexArray(0);
	}

	void onResize(int w, int h)
	{
		application::onResize(w, h);
	}

	void render(double currentTime)
	{
		glBindVertexArray(_vao);
		const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, bckColor);
		glClearBufferfv(GL_DEPTH, 0, white);

		vmath::mat4 mvp = vmath::perspective(35.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0) 
			* vmath::lookat(vmath::vec3(0.0, 0.0f, -5.0f), vmath::vec3(0.0, 0.0f, 0.0f), vmath::vec3(0.0, 1.0f, 0.0f))
			* vmath::translate(0.0f, 0.0f, 5.0f);
		glUniformMatrix4fv(3, 1, GL_FALSE, mvp);
		//		glClear(GL_DEPTH_BUFFER_BIT);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr, 4);
	}

	void shutdown()
	{
		glDeleteProgram(_renderingProgram);

		glDisableVertexAttribArray(0);

		glBindVertexArray(0);
		glDeleteVertexArrays(1, &_vao);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &_vertsBuffer);
		glDeleteBuffers(1, &_elementsBuffer);
	}

private:
	GLuint _renderingProgram;
	GLuint _vao;
	GLuint _vertsBuffer;
	GLuint _elementsBuffer;
	GLuint _mvpLocation;
	vmath::mat4 projMatrix;
	GLfloat mvLocation = 2;
	GLfloat projLocation = 3;

};

DECLARE_MAIN(InstancingDivisor);

