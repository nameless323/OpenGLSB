#include <sb6.h>
#include "vmath.h"
#include "ShaderProgram.h"
#include <object.h>

using vmath::mat4;

class ColorBlend : public sb6::application
{
public:
	void startup()
	{
		//		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
		_shader.CreateProgram();
		_shader.AttachShader("Shaders/ColorBlend/ColorBlend.vert");
		_shader.AttachShader("Shaders/ColorBlend/ColorBlend.frag");
		_shader.Link();

		static const GLfloat vertices[] =
		{
			-0.25f, -0.25f, -0.25f,
			-0.25f,  0.25f, -0.25f,
			0.25f, -0.25f, -0.25f,
			0.25f,  0.25f, -0.25f,
			0.25f, -0.25f,  0.25f,
			0.25f,  0.25f,  0.25f,
			-0.25f, -0.25f,  0.25f,
			-0.25f,  0.25f,  0.25f,
		};

		static const GLushort indices[] =
		{
			0, 1, 2,
			2, 1, 3,
			2, 3, 4,
			4, 3, 5,
			4, 5, 6,
			6, 5, 7,
			6, 7, 0,
			0, 7, 1,
			6, 0, 2,
			2, 4, 6,
			7, 5, 3,
			7, 3, 1
		};

		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glGenBuffers(1, &_vertsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _vertsBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &_elementsBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementsBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		_mvLocation = glGetUniformLocation(_shader.GetHandler(), "mv_matrix");
		_projLocation = glGetUniformLocation(_shader.GetHandler(), "proj_matrix");
		
		_shader.Use();
	}

	void onResize(int w, int h)
	{
		application::onResize(w, h);
	}

	void render(double currentTime)
	{
		static const GLfloat orange[] = { 0.6f, 0.4f, 0.1f, 1.0f };
		static const GLfloat one = 1.0f;

		static const GLenum blendFunc[] =
		{
			GL_ZERO,
			GL_ONE,
			GL_SRC_COLOR,
			GL_ONE_MINUS_SRC_COLOR,
			GL_DST_COLOR,
			GL_ONE_MINUS_DST_COLOR,
			GL_SRC_ALPHA,
			GL_ONE_MINUS_SRC_ALPHA,
			GL_DST_ALPHA,
			GL_ONE_MINUS_DST_ALPHA,
			GL_CONSTANT_COLOR,
			GL_ONE_MINUS_CONSTANT_COLOR,
			GL_CONSTANT_ALPHA,
			GL_ONE_MINUS_CONSTANT_ALPHA,
			GL_SRC_ALPHA_SATURATE,
			GL_SRC1_COLOR,
			GL_ONE_MINUS_SRC1_COLOR,
			GL_SRC1_ALPHA,
			GL_ONE_MINUS_SRC1_ALPHA
		};
		static const int numBlendFunc = sizeof(blendFunc) / sizeof(blendFunc[0]);
		static const float xScale = 20.0f / float(numBlendFunc);
		static const float yScale = 16.0f / float(numBlendFunc);
		const float t = (float)currentTime;

		glViewport(0, 0, info.windowWidth, info.windowHeight);
		glClearBufferfv(GL_COLOR, 0, orange);
		glClearBufferfv(GL_DEPTH, 0, &one);
		_shader.Use();

		mat4 proj = vmath::perspective(50.f, (float)info.windowWidth / (float)info.windowHeight, 0.1f, 1000.0f);
		glUniformMatrix4fv(_projLocation, 1, GL_FALSE, proj);

		glEnable(GL_BLEND);
		glBlendColor(0.2f, 0.5f, 0.7f, 0.5f);
		for (int j = 0; j < numBlendFunc; j++)
		{
			for (int i = 0; i < numBlendFunc; i++)
			{
				mat4 mv = vmath::translate(9.5f - xScale * float(i), 7.5f - yScale * float(j), -18.0f) *
					vmath::rotate(t * -45.0f, 0.0f, 1.0f, 0.0f) *
					vmath::rotate(t * -21.0f, 1.0f, 0.0f, 0.0f);
				glUniformMatrix4fv(_mvLocation, 1, GL_FALSE, mv);
				glBlendFunc(blendFunc[i], blendFunc[j]);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
			}
		}
	}

	void shutdown()
	{
		_shader.DeleteProgram();
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &_vao);
		glDeleteBuffers(1, &_elementsBuffer);
		glDeleteBuffers(1, &_vertsBuffer);
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
	GLint _projLocation;
	GLint _normalLengthLocation;
	GLint _viewpointLocation;

	sb6::object _object;
};

//DECLARE_MAIN(ColorBlend);

