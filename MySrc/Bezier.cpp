#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"
#include "ShaderProgram.h"

class Bezier : public sb6::application
{
public:
	Bezier () : _showPoints(false), _showCage(false), _wireframe(false), _paused(false)
	{}

	void startup()
	{
		//		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
		_renderingProgram.CreateProgram();
		_renderingProgram.AttachShader("Shaders/Tesselation/Bezier/Bezier.vert");
		_renderingProgram.AttachShader("Shaders/Tesselation/Bezier/Bezier.tesc");
		_renderingProgram.AttachShader("Shaders/Tesselation/Bezier/Bezier.tese");
		_renderingProgram.AttachShader("Shaders/Tesselation/Bezier/Bezier.frag");
		_renderingProgram.Link();

		_uniforms.Patch.MV = glGetUniformLocation(_renderingProgram.GetHandler(), "mv_matrix");
		_uniforms.Patch.Proj = glGetUniformLocation(_renderingProgram.GetHandler(), "proj_matrix");
		_uniforms.Patch.MVP = glGetUniformLocation(_renderingProgram.GetHandler(), "mvp");

		_drawControlPointsProgram.CreateProgram();
		_drawControlPointsProgram.AttachShader("Shaders/Tesselation/Bezier/ControlPoints.vert");
		_drawControlPointsProgram.AttachShader("Shaders/Tesselation/Bezier/ControlPoints.frag");
		_drawControlPointsProgram.Link();

		_uniforms.ControlPoint.DrawColor = glGetUniformLocation(_drawControlPointsProgram.GetHandler(), "draw_color");
		_uniforms.ControlPoint.MVP = glGetUniformLocation(_drawControlPointsProgram.GetHandler(), "mvp");


		glGenVertexArrays(1, &_patchVAO);
		glBindVertexArray(_patchVAO);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glGenBuffers(1, &_patchVBO);
		glBindBuffer(GL_ARRAY_BUFFER, _patchVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(_patchData), nullptr, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);

		const GLushort indices[] =
		{
			0, 1, 1, 2, 2, 3,
			4, 5, 5, 6, 6, 7,
			8, 9, 9, 10, 10, 11,
			12, 13, 13, 14, 14, 15,

			0, 4, 4, 8, 8, 12,
			1, 5, 5, 9, 9, 13,
			2, 6, 6, 10, 10, 14,
			3, 7, 7, 11, 11, 15
		};
		glGenBuffers(1, &_cageIndices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _cageIndices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		_renderingProgram.Use();
	}

	void onKey(int key, int action)
	{
		if (action)
		{
			switch (key)
			{
			case 'C': _showCage = !_showCage;
				break;
			case 'X': _showPoints = !_showPoints;
				break;
			case 'W': _wireframe = !_wireframe;
				break;
			case 'P': _paused = !_paused;
				break;
			default:
				break;
			}
		}
	}

	void onResize(int w, int h)
	{
		application::onResize(w, h);
	}

	void render(double currentTime)
	{

		const GLfloat bckColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
		const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, bckColor);
		glClearBufferfv(GL_DEPTH, 0, white);

		int i;
		static double _lastTime = 0.0;
		static double _totalTime = 0.0;

		if (!_paused)
			_totalTime += (currentTime - _lastTime);
		_lastTime = currentTime;

		float t = (float)_totalTime;

		static const float patchInitializer[] =
		{
			-1.0f,  -1.0f,  0.0f,
			-0.33f, -1.0f,  0.0f,
			0.33f, -1.0f,  0.0f,
			1.0f,  -1.0f,  0.0f,

			-1.0f,  -0.33f, 0.0f,
			-0.33f, -0.33f, 0.0f,
			0.33f, -0.33f, 0.0f,
			1.0f,  -0.33f, 0.0f,

			-1.0f,   0.33f, 0.0f,
			-0.33f,  0.33f, 0.0f,
			0.33f,  0.33f, 0.0f,
			1.0f,   0.33f, 0.0f,

			-1.0f,   1.0f,  0.0f,
			-0.33f,  1.0f,  0.0f,
			0.33f,  1.0f,  0.0f,
			1.0f,   1.0f,  0.0f,
		};
		vmath::vec3 *p = (vmath::vec3*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(_patchData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		memcpy(p, patchInitializer, sizeof(patchInitializer));

		for (int i = 0; i < 16; i++)
		{
			float fi = (float)i / 16.0f;
			p[i][2] = sinf(t * (0.2f + fi * 0.3f));
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);

		_renderingProgram.Use();
		vmath::mat4 projMatrix = vmath::perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight, 1.0f, 1000.0f);
		vmath::mat4 mvMatrix = vmath::translate(0.0f, 0.0f, -4.0f)*
			vmath::rotate(t * 10.0f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate(t * 17.0f, 1.0f, 0.0f, 0.0f);

		glUniformMatrix4fv(_uniforms.Patch.MV, 1, GL_FALSE, mvMatrix);
		glUniformMatrix4fv(_uniforms.Patch.Proj, 1, GL_FALSE, projMatrix);
		glUniformMatrix4fv(_uniforms.Patch.MVP, 1, GL_FALSE, projMatrix * mvMatrix);

		if (_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glPatchParameteri(GL_PATCH_VERTICES, 16);
		glDrawArrays(GL_PATCHES, 0, 16);

		_drawControlPointsProgram.Use();
		glUniformMatrix4fv(_uniforms.ControlPoint.MVP, 1, GL_FALSE, projMatrix * mvMatrix);

		if (_showPoints)
		{
			glPointSize(9.0f);
			glUniform4fv(_uniforms.ControlPoint.DrawColor, 1, vmath::vec4(0.2f, 0.7f, 0.9f, 1.0f));
			glDrawArrays(GL_POINTS, 0, 16);
		}

		if (_showCage)
		{
			glUniform4fv(_uniforms.ControlPoint.DrawColor, 1, vmath::vec4(0.7, 0.9, 0.2f, 1.0f));
			glDrawElements(GL_LINES, 48, GL_UNSIGNED_SHORT, nullptr);
		}
	}

	void shutdown()
	{
		_renderingProgram.DeleteProgram();
		_drawControlPointsProgram.DeleteProgram();

		glDisableVertexAttribArray(0);

		glBindVertexArray(0);
		glDeleteVertexArrays(1, &_patchVAO);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &_patchVBO);
		glDeleteBuffers(1, &_cageIndices);
	}

private:
	ShaderProgram _renderingProgram;
	ShaderProgram _drawControlPointsProgram;

	GLuint _patchVAO;
	GLuint _patchVBO;
	GLuint _cageIndices;
	vmath::vec3 _patchData[16];

	bool _showPoints;
	bool _showCage;
	bool _wireframe;
	bool _paused;

	struct
	{
		struct
		{
			int MV;
			int Proj;
			int MVP;
		} Patch;
		struct
		{
			int DrawColor;
			int MVP;
		} ControlPoint;
	} _uniforms;
};

//DECLARE_MAIN(Bezier);

