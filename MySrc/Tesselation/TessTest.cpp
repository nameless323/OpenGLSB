#include <sb6.h>
#include "vmath.h"
#include "../ShaderProgram.h"

class TessTest : public sb6::application
{
public:
	void startup()
	{
		//		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
		_renderingProgramQuad.CreateProgram();
		_renderingProgramQuad.AttachShader("Shaders/Tesselation/Test/TessTest.vert");
		_renderingProgramQuad.AttachShader("Shaders/Tesselation/Test/TessTestQuad.tesc");
		_renderingProgramQuad.AttachShader("Shaders/Tesselation/Test/TessTestQuad.tese");
		_renderingProgramQuad.AttachShader("Shaders/Tesselation/Test/TessTest.frag");
		_renderingProgramQuad.Link();
//
		_renderingProgramTrig.CreateProgram();
		_renderingProgramTrig.AttachShader("Shaders/Tesselation/Test/TessTest.vert");
		_renderingProgramTrig.AttachShader("Shaders/Tesselation/Test/TessTestTrig.tesc");
		_renderingProgramTrig.AttachShader("Shaders/Tesselation/Test/TessTestTrig.tese");
		_renderingProgramTrig.AttachShader("Shaders/Tesselation/Test/TessTest.frag");
		_renderingProgramTrig.Link();


		glGenVertexArrays(2, _vao);
		glBindVertexArray(_vao[0]);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
//		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glGenBuffers(2, _vertsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer[0]);
		const GLfloat vertsQd[] =
		{
			-0.5f, -0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,
			0.0f, 0.5f, 0.5f,
			0.0f, -0.5f, 0.5f
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertsQd), vertsQd, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		const GLubyte indicesQd[] =
		{
			0, 1, 2,
			0, 2, 3
		};
		glGenBuffers(1, &_elementsBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementsBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesQd), indicesQd, GL_STATIC_DRAW);


		glBindVertexArray(_vao[1]);
		const GLfloat vertsTrig[] =
		{
			0.6f, -0.5f, 0.5f,
			0.6f, 0.5f, 0.5f,
			0.1f, 0.5f, 0.5f
		};
		glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertsTrig), vertsTrig, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);

		_renderingProgramQuad.Use();
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
		//		glClear(GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(_vao[0]);
		_renderingProgramQuad.Use();

		glPatchParameteri(GL_PATCH_VERTICES, 6);
		glDrawElements(GL_PATCHES, 6, GL_UNSIGNED_BYTE, nullptr);
		_renderingProgramTrig.Use();
		glBindVertexArray(_vao[1]);

		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawArrays(GL_PATCHES, 0, 3);
	}

	void shutdown()
	{
		_renderingProgramQuad.DeleteProgram();

		glDisableVertexAttribArray(0);

		glBindVertexArray(0);
		glDeleteVertexArrays(2, _vao);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(2, _vertsBuffer);
		glDeleteBuffers(1, &_elementsBuffer);
	}

private:
	ShaderProgram _renderingProgramQuad;
	ShaderProgram _renderingProgramTrig;
	GLuint _vao[2];
	GLuint _vertsBuffer[2];
	GLuint _elementsBuffer;

	vmath::mat4 projMatrix;
	GLfloat mvLocation = 2;
	GLfloat projLocation = 3;

};

//DECLARE_MAIN(TessTest);

