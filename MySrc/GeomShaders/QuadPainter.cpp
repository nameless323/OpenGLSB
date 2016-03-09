#include <sb6.h>
#include "vmath.h"
#include "../ShaderProgram.h"
#include <object.h>

using vmath::mat4;

class QuadPainter : public sb6::application
{
public:
	QuadPainter() : _mode (0), _paused(false), _vidOffset(0)
	{		
	}

	void startup()
	{
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		//		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
		_shaderLinesAdjacency.CreateProgram();
		_shaderLinesAdjacency.AttachShader("Shaders/GeomShaders/QuadPainter/QuadPainter.vert");
		_shaderLinesAdjacency.AttachShader("Shaders/GeomShaders/QuadPainter/QuadPainter.geom");
		_shaderLinesAdjacency.AttachShader("Shaders/GeomShaders/QuadPainter/QuadPainter.frag");
		_shaderLinesAdjacency.Link();

		_mvpLocLinesAdj = glGetUniformLocation(_shaderLinesAdjacency.GetHandler(), "mvp");
		_vidOffsetLinesAdj = glGetUniformLocation(_shaderLinesAdjacency.GetHandler(), "vid_offset");

		_shaderFans.CreateProgram();
		_shaderFans.AttachShader("Shaders/GeomShaders/QuadPainter/QuadPainterFans.vert");
		_shaderFans.AttachShader("Shaders/GeomShaders/QuadPainter/QuadPainterFans.frag");
		_shaderFans.Link();

		_mvpLocFans = glGetUniformLocation(_shaderFans.GetHandler(), "mvp");
		_vidOffsetFans = glGetUniformLocation(_shaderFans.GetHandler(), "vid_offset");

		//glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

	}

	void onResize(int w, int h)
	{
		application::onResize(w, h);
	}

	void render(double currentTime)
	{

		const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };

		glViewport(0, 0, info.windowWidth, info.windowHeight);
		glClearBufferfv(GL_COLOR, 0, bckColor);
		glClearBufferfv(GL_DEPTH, 0, white);
		
		static double lastTime = 0.0;
		static double totalTime = 0.0;

		if (!_paused)
			totalTime += (currentTime - lastTime);
		lastTime = currentTime;
		
		float t = (float)totalTime;
		mat4 mvMat = vmath::translate(0.0f, 0.0f, -2.0f) *
			vmath::rotate((float)t * 5.0f, 0.0f, 0.0f, 1.0f) *
			vmath::rotate((float)t * 30.0f, 1.0f, 0.0f, 0.0f);
		mat4 projMat = vmath::perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);
		mat4 mvp = projMat * mvMat;

		switch (_mode)
		{
		case 0:
			_shaderFans.Use();
			glUniformMatrix4fv(_mvpLocFans, 1, GL_FALSE, mvp);
			glUniform1i(_vidOffsetFans, _vidOffset);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			break;

		case 1:
			_shaderLinesAdjacency.Use();
			glUniformMatrix4fv(_mvpLocLinesAdj, 1, GL_FALSE, mvp);
			glUniform1i(_vidOffsetLinesAdj, _vidOffset);
			glDrawArrays(GL_LINES_ADJACENCY, 0, 4);
			break;
		}
	}

	void onKey(int key, int action) override
	{
		if (!action)
			return;
		switch (key)
		{
		case '1':
		case '2':
			_mode = key - '1';
			break;
		case GLFW_KEY_KP_ADD:
			_vidOffset++;
			break;
		case GLFW_KEY_KP_SUBTRACT:
			_vidOffset--;
			break;
		case 'P': _paused = !_paused;
			break;
		case 'M':
			_mode = (_mode + 1) % 2;
			break;
		}
	}

	void shutdown()
	{
		_shaderLinesAdjacency.DeleteProgram();
		_shaderFans.DeleteProgram();
		glDeleteVertexArrays(1, &_vao);
	}

private:
	ShaderProgram _shaderLinesAdjacency;
	ShaderProgram _shaderFans;
	GLuint _vao;
	GLuint _vertsBuffer[2];
	GLuint _elementsBuffer;

	mat4 _projMatrix;
	GLint _mvpLocFans = 2;
	GLint _vidOffsetFans = 3;
	GLint _mvpLocLinesAdj;
	GLint _vidOffsetLinesAdj;

	int _vidOffset;
	int _mode;
	bool _paused;
};

//DECLARE_MAIN(QuadPainter);

