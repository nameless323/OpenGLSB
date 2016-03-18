#include <sb6.h>
#include "vmath.h"
#include "ShaderProgram.h"
#include <object.h>

using vmath::mat4;

class FBOLayeredTex : public sb6::application
{
public:
	void startup()
	{
		//		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
		_layersShader.CreateProgram();
		_layersShader.AttachShader("Shaders/FBOLayeredTex/FBOLayeredTex.vert");
		_layersShader.AttachShader("Shaders/FBOLayeredTex/FBOLayeredTex.geom");
		_layersShader.AttachShader("Shaders/FBOLayeredTex/FBOLayeredTex.frag");
		_layersShader.Link();

		_showLayersShader.CreateProgram();
		_showLayersShader.AttachShader("Shaders/FBOLayeredTex/FBOLayeredTexShow.vert");
		_showLayersShader.AttachShader("Shaders/FBOLayeredTex/FBOLayeredTexShow.frag");
		_showLayersShader.Link();

		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		_object.load("media/objects/torus.sbm");

		glGenBuffers(1, &_transformUbo);
		glBindBuffer(GL_UNIFORM_BUFFER, _transformUbo);
		glBufferData(GL_UNIFORM_BUFFER, 17 * sizeof(vmath::mat4), nullptr, GL_DYNAMIC_DRAW);

		glGenTextures(1, &_arrayTex);
		glBindTexture(GL_TEXTURE_2D_ARRAY, _arrayTex);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 256, 256, 16);

		glGenTextures(1, &_depthTex);
		glBindTexture(GL_TEXTURE_2D_ARRAY, _depthTex);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32, 256, 256, 16);

		glGenFramebuffers(1, &_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _arrayTex, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthTex, 0);
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void onResize(int w, int h)
	{
		application::onResize(w, h);
	}

	void render(double currentTime)
	{

		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		static const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };

		mat4 proj = vmath::perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);

		float f = (float)currentTime * 0.3f;
		mat4 mv = vmath::translate(0.0f, 0.0f, -4.0f)*
			vmath::rotate((float)currentTime * 5.0f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate((float)currentTime * 30.0f, 1.0f, 0.0f, 0.0f);
		mat4 mvp = proj * mv;

		struct TRANSFORM_BUFFER
		{
			mat4 proj;
			mat4 mv[16];
		};

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, _transformUbo);

		TRANSFORM_BUFFER* buffer = (TRANSFORM_BUFFER*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(TRANSFORM_BUFFER), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		buffer->proj = vmath::perspective(50.0f, 1.0f, 0.1f, 1000.0f);
		
		for (int i = 0; i < 16; i++)
		{
			float fi = (float)(i + 12) / 16.0f;
			buffer->mv[i] = vmath::translate(0.0f, 0.0f, -4.0f) *
				vmath::rotate((float)currentTime * 25.0f * fi, 0.0f, 0.0f, 1.0f) *
				vmath::rotate((float)currentTime * 30.0f * fi, 1.0f, 0.0f, 0.0f);
		}
		glUnmapBuffer(GL_UNIFORM_BUFFER);

		static const GLenum ca0 = GL_COLOR_ATTACHMENT0;
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
		glDrawBuffers(1, &ca0);
		glViewport(0, 0, 256, 256);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, white);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		_layersShader.Use();
		_object.render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);
		_showLayersShader.Use();

		glViewport(0, 0, info.windowWidth, info.windowHeight);
		glClearBufferfv(GL_COLOR, 0, gray);

//		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, _arrayTex);
		glDisable(GL_DEPTH_TEST);

		glBindVertexArray(_vao);
		glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 16);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	}

	void shutdown()
	{
		_layersShader.DeleteProgram();
		_showLayersShader.DeleteProgram();
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &_vao);
		glDeleteBuffers(1, &_elementsBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &_fbo);
		glDeleteTextures(1, &_arrayTex);
		glDeleteTextures(1, &_depthTex);
	}

private:
	ShaderProgram _layersShader;
	ShaderProgram _showLayersShader;
	GLuint _vao;
	GLuint _transformUbo;
	GLuint _elementsBuffer;
	GLuint _fbo;
	GLuint _arrayTex;
	GLuint _depthTex;

	mat4 _projMatrix;
	GLint _mvLocation = 2;
	GLint _projLocation;
	GLint _mvLocation2 = 2;
	GLint _projLocation2 = 3;
	GLint _explodeFactor;
	GLint _normalLengthLocation;
	GLint _viewpointLocation;

	sb6::object _object;
};

DECLARE_MAIN(FBOLayeredTex);

