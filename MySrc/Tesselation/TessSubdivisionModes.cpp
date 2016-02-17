#include <sb6.h>
#include "vmath.h"
#include "../ShaderProgram.h"

using vmath::mat4;
using vmath::translate;
using vmath::rotate;
using vmath::perspective;

//#define MANY_CUBES
class TessSubdivsionModes : public sb6::application
{
public:
	void startup()
	{
		_index = 0;
		std::string fsFilename = "Shaders/Tesselation/TessSubdivModes/TessSubdivModes.frag";
		std::string vsFilename = "Shaders/Tesselation/TessSubdivModes/TessSubdivModes.vert";
		std::string tescFilename = "Shaders/Tesselation/TessSubdivModes/TessSubdivModes.tesc";
		std::string teseFilename[] =
		{
			"Shaders/Tesselation/TessSubdivModes/TessSubdivModesEqual.tese",
			"Shaders/Tesselation/TessSubdivModes/TessSubdivModesFractEven.tese",
			"Shaders/Tesselation/TessSubdivModes/TessSubdivModesFractOdd.tese"
		};

		for (int i = 0; i < 3; i++)
		{
			_shaders[i].CreateProgram();
			_shaders[i].AttachShader(vsFilename);
			_shaders[i].AttachShader(tescFilename);
			_shaders[i].AttachShader(teseFilename[i]);
			_shaders[i].AttachShader(fsFilename);
			_shaders[i].Link();
		}

		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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

		_shaders[_index].Use();
		 glUniform1f(0, sinf((float)currentTime) * 5.0f + 6.0f);
//		glUniform1f(0, 5.3f);
		glDrawArrays(GL_PATCHES, 0, 4);
	}

	void shutdown()
	{
		for (int i = 0; i < 3; i++)
			_shaders[i].DeleteProgram();

		glBindVertexArray(0);
		glDeleteVertexArrays(1, &_vao);
	}

	virtual void onKey(int key, int action)
	{
		if (action == 1)
		{
			switch (key)
			{
			case 'M': _index = (_index + 1) % 3;
				break;
			}
		}
	}

private:
	ShaderProgram _shaders[3];
	GLuint _vao;
	int _index;
};

//DECLARE_MAIN(TessSubdivsionModes);

