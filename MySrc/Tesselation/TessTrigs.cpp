#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "../Utils.h"
#include "vmath.h"
#include "../ShaderProgram.h"

class TessTrigs : public sb6::application
{
public:
	void startup()
	{
		_renderingProgram.CreateProgram();
		_renderingProgram.AttachShader("Shaders/Tesselation/Triangle/TessTrig.vert");
		_renderingProgram.AttachShader("Shaders/Tesselation/Triangle/TessTrig.frag");
		_renderingProgram.AttachShader("Shaders/Tesselation/Triangle/TessTrig.tesc");
		_renderingProgram.AttachShader("Shaders/Tesselation/Triangle/TessTrig.tese");
		_renderingProgram.Link();
		
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		_renderingProgram.Use();
	}

	void onResize(int w, int h)
	{
		application::onResize(w, h);
	}

	void render(double currentTime)
	{

		const GLfloat bckColor[] = { 0.0f, 0.25f, 0.0f, 1.0f };
		const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, bckColor);
		glClearBufferfv(GL_DEPTH, 0, white);
		//		glClear(GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_PATCHES, 0, 3);
	}

	void shutdown()
	{
		_renderingProgram.DeleteProgram();
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &_vao);
	}

private:
	GLuint _vao;
	GLuint _vertsBuffer;
	GLuint _elementsBuffer;

	vmath::mat4 projMatrix;
	GLfloat mvLocation = 2;
	GLfloat projLocation = 3;
	ShaderProgram _renderingProgram;
};

DECLARE_MAIN(TessTrigs);

