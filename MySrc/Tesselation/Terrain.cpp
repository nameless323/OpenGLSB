#include <sb6.h>
#include "vmath.h"
#include "../ShaderProgram.h"

class Terrain : public sb6::application
{
public:
	Terrain() : _enableDisplacement(true), _wireframe(false), _enableLog(true), _paused(false)
	{}

	void startup()
	{
		//		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
		_renderingProgram.CreateProgram();
		_renderingProgram.AttachShader("Shaders/Common/Default.vert");
		_renderingProgram.AttachShader("Shaders/Common/Default.frag");
		_renderingProgram.Link();
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		_renderingProgram.Use();
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
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
	}

	void shutdown()
	{
		_renderingProgram.DeleteProgram();

		glDisableVertexAttribArray(0);

		glBindVertexArray(0);
		glDeleteVertexArrays(1, &_vao);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

private:
	ShaderProgram _renderingProgram;
	GLuint _vao;
	GLuint _displacementTex;
	GLuint _mainTex;

	float _displacementDepth;
	bool _enableDisplacement;
	bool _wireframe;
	bool _enableLog;
	bool _paused;

	struct
	{
		GLint MVP;
		GLint Proj;
		GLint MV;
		GLint DispDepth;
		GLint EnableFog;
	} _uniforms;
};

//DECLARE_MAIN(Dummy);

