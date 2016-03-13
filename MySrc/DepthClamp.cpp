#include <sb6.h>
#include "vmath.h"
#include "ShaderProgram.h"
#include <object.h>

using vmath::mat4;

class DepthClamp : public sb6::application
{
public:
	void startup()
	{
		//		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
		_shader.CreateProgram();
		_shader.AttachShader("Shaders/DepthClamp/DepthClamp.vert");
		_shader.AttachShader("Shaders/DepthClamp/DepthClamp.frag");
		_shader.Link();

		_mvLocation = glGetUniformLocation(_shader.GetHandler(), "mv_matrix");
		_projLocation = glGetUniformLocation(_shader.GetHandler(), "proj_matrix");

		_object.load("media/objects/dragon.sbm");

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		_shader.Use();
	}

	void onResize(int w, int h)
	{
		application::onResize(w, h);
	}

	void render(double currentTime)
	{
		glViewport(0, 0, info.windowWidth, info.windowHeight);
		float f = 0.9f;

		static const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, bckColor);
		glClearBufferfv(GL_DEPTH, 0, white);
		_shader.Use();

		vmath::mat4 proj = vmath::perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight, 1.8f, 1000.0f);
		glUniformMatrix4fv(_projLocation, 1, GL_FALSE, proj);
		glEnable(GL_DEPTH_CLAMP);

		mat4 mv = vmath::translate(0.0f, 0.0f, -3.0f) *
			vmath::rotate(f * 45.0f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate(f * 81.0f, 1.0f, 0.0f, 0.0f) * vmath::scale(0.2f, 0.2f, 0.2f);
		glUniformMatrix4fv(_mvLocation, 1, GL_FALSE, mv);

		_object.render();
	}

	void shutdown()
	{
		_shader.DeleteProgram();
		_object.free();
	}

private:
	ShaderProgram _shader;
	GLuint _vao;
	GLuint _vertsBuffer;
	GLuint _elementsBuffer;
	GLuint _uniformBuffer;

	mat4 _projMatrix;
	GLint _mvLocation = 2;
	GLint _mvpLocation = 3;
	GLint _explodeFactor;
	GLint _projLocation;
	GLint _normalLengthLocation;
	GLint _viewpointLocation;

	sb6::object _object;
};

//DECLARE_MAIN(DepthClamp);

