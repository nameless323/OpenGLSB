#include <sb6.h>

#include <object.h>

#include "vmath.h"
#include "../ShaderProgram.h"

namespace OpenGlSB
{
using vmath::mat4;

class GeomShaderCulling : public sb6::application
{
public:
	void startup()
	{
		//		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
		_shader.CreateProgram();
		_shader.AttachShader("Shaders/GeomShaders/Culling/GeomCulling.vert");
		_shader.AttachShader("Shaders/GeomShaders/Culling/GeomCulling.geom");
		_shader.AttachShader("Shaders/GeomShaders/Culling/GeomCulling.frag");
		_shader.Link();

		_mvLocation = glGetUniformLocation(_shader.GetHandler(), "mvMatrix");
		_mvpLocation = glGetUniformLocation(_shader.GetHandler(), "mvpMatrix");
		_viewpointLocation = glGetUniformLocation(_shader.GetHandler(), "viewpoint");

		_object.load("media/objects/dragon.sbm");

		glDisable(GL_CULL_FACE);
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

		const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, bckColor);
		glClearBufferfv(GL_DEPTH, 0, white);
		_shader.Use();

		mat4 projMatrix = vmath::perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);
		mat4 mvMatrix = vmath::translate(0.0f, -2.0f, -20.5f);

		glUniformMatrix4fv(_mvpLocation, 1, GL_FALSE, projMatrix * mvMatrix);
		glUniformMatrix4fv(_mvLocation, 1, GL_FALSE, mvMatrix);

		float f = static_cast<float>(currentTime);
		GLfloat viewpoint[] = 
		{
			sinf(f * 2.1f) * 70.0f,
			cosf(f * 1.4f) * 70.0f,
			sinf(f * 0.7f) * 70.0f
		};
		glUniform3fv(_viewpointLocation, 1, viewpoint);
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
	GLuint _vertsBuffer[2];
	GLuint _elementsBuffer;

	mat4 _projMatrix;
	GLint _mvLocation = 2;
	GLint _mvpLocation = 3;
	GLint _viewpointLocation;

	sb6::object _object;
};
}
//DECLARE_MAIN(GeomShaderCulling);

