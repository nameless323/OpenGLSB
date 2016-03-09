#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"
#include "ShaderProgram.h"

class NoPerspective : public sb6::application
{
public:
	NoPerspective() : _paused(false), _usePerspective(false)
	{}

	void startup()
	{
		_renderingProgram.CreateAndLinkProgram("Shaders/NoPerspective/NoPerspective.vert", "Shaders/NoPerspective/NoPerspective.frag");
		
		_uniforms.MVP = glGetUniformLocation(_renderingProgram.GetHandler(), "mvp");
		_uniforms.UsePerspective = glGetUniformLocation(_renderingProgram.GetHandler(), "use_perspective");
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		static const unsigned char checkerData[] =
		{
			0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
			0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
			0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
			0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
			0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
			0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
			0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
			0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00
		};

		glGenTextures(1, &_texChecker);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texChecker);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, 8, 8);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 8, 8, GL_RED, GL_UNSIGNED_BYTE, checkerData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		_renderingProgram.Use();
	}

	void onResize(int w, int h)
	{
		application::onResize(w, h);
	}

	void render(double currentTime)
	{

		glViewport(0, 0, info.windowWidth, info.windowHeight);
		const GLfloat bckColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, bckColor);
		glClearBufferfv(GL_DEPTH, 0, white);

		static double lastTime = 0.0;
		static double totalTime = 0.0;

		if (!_paused)
			totalTime += currentTime - lastTime;
		lastTime = currentTime;
		float t = (float)totalTime * 14.3f;

		vmath::mat4 mv = vmath::translate(0.0f, 0.0f, -1.5f) *
			vmath::rotate(t, 0.0f, 1.0f, 0.0f);
		vmath::mat4 proj = vmath::perspective(60.0f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);

		glUniformMatrix4fv(_uniforms.MVP, 1, GL_FALSE, proj * mv);
		glUniform1i(_uniforms.UsePerspective, _usePerspective);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	void shutdown()
	{
		_renderingProgram.DeleteProgram();

		glDisableVertexAttribArray(0);

		glBindVertexArray(0);
		glDeleteVertexArrays(1, &_vao);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void onKey(int key, int action) override
	{
		if (action == 1)
		{
			switch(key)
			{
			case 'I': _usePerspective = !_usePerspective;
				break;
			case 'P': _paused = !_paused;
				break;
			default:
				break;
			}
		}
	}

private:
	ShaderProgram _renderingProgram;
	GLuint _vao;
	GLuint _texChecker;
	bool _paused;
	bool _usePerspective;

	struct
	{
		GLint MVP;
		GLint UsePerspective;
	} _uniforms;
};

DECLARE_MAIN(NoPerspective);

