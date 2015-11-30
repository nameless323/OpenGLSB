#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <shader.h>
#include "Utils.h"
#include "vmath.h"
#include <object.h>

using vmath::vec4;
using vmath::vec3;
using vmath::mat4;

enum 
{ NUM_DRAWS = 50000 };

struct DrawArraysIndirectCommand
{
	GLuint Count;
	GLuint PrimCount;
	GLuint First;
	GLuint BaseInstance;
};

class IndirectDraw : public sb6::application
{
public:
	IndirectDraw() : _renderProgram(0), _mode(MODE_MULTIDRAW), _paused(false), _vsync(false)
	{}


	void startup() override
	{
		int i;
		LoadShaders();
		_obj.load("media/objects/asteroids.sbm");

		glGenBuffers(1, &_indirectDrawBuffer);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _indirectDrawBuffer);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawArraysIndirectCommand)*NUM_DRAWS, nullptr, GL_STATIC_DRAW);

		DrawArraysIndirectCommand* cmd = (DrawArraysIndirectCommand*)glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, NUM_DRAWS*sizeof(DrawArraysIndirectCommand), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		for (int i = 0; i < NUM_DRAWS; i++)
		{
			_obj.get_sub_object_info(i % _obj.get_sub_object_count(),
				cmd[i].First, cmd[i].Count);
			cmd[i].PrimCount = 1;
			cmd[i].BaseInstance = i;
		}
		glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);

		glBindVertexArray(_obj.get_vao());
		glGenBuffers(1, &_drawIndexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _drawIndexBuffer);
		glBufferData(GL_ARRAY_BUFFER, NUM_DRAWS*sizeof(GLuint), nullptr, GL_STATIC_DRAW);

		GLuint* drawIndex = (GLuint*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(GLuint)*NUM_DRAWS, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		for (int i = 0; i < NUM_DRAWS; i++)
		{
			drawIndex[i] = i;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);

		glVertexAttribIPointer(10, 1, GL_UNSIGNED_INT, 0, nullptr);
		glVertexAttribDivisor(10, 1);
		glEnableVertexAttribArray(10);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glEnable(GL_CULL_FACE);
	}

	void onResize(int w, int h)
	{
		application::onResize(w, h);
	}

	void render(double currentTime)
	{
		int j;
		static const float one = 1.0f;
		static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		static double lastTime = 0.0;
		static double totalTime = 0.0;

		if (!_paused)
			totalTime += (currentTime - lastTime);

		lastTime = currentTime;

		float t = float(totalTime);
		int i = int(totalTime * 3.0f);
		glViewport(0, 0, info.windowWidth, info.windowHeight);
		glClearBufferfv(GL_COLOR, 0, black);
		glClearBufferfv(GL_DEPTH, 0, &one);

		const mat4 viewMat = lookat(vec3(100.0f * cosf(t * 0.023f), 100.0f * cosf(t * 0.023f), 300.0f * sinf(t * 0.037f) - 600.0f),
			vec3(0.0f, 0.0f, 260.0f),
			normalize(vec3(0.1f - cosf(t * 0.1f) * 0.3f, 1.0f, 0.0f)));
		const mat4 projMatrix = vmath::perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight, 1.0f, 2000.0f);
		glUseProgram(_renderProgram);

		glUniform1f(_uniforms.Time, t);
		glUniformMatrix4fv(_uniforms.ViewMatrix, 1, GL_FALSE, viewMat);
		glUniformMatrix4fv(_uniforms.ProjMatrix, 1, GL_FALSE, projMatrix);
		glUniformMatrix4fv(_uniforms.ViewProjMatrix, 1, GL_FALSE, projMatrix*viewMat);
		glBindVertexArray(_obj.get_vao());

		if (_mode == MODE_MULTIDRAW)
		{
			glMultiDrawArraysIndirect(GL_TRIANGLES, nullptr, NUM_DRAWS, 0);
		}
		else if (_mode == MODE_SEPARATE_DRAWS)
		{
			for (j = 0; j < NUM_DRAWS; j++)
			{
				GLuint first, cout;
				_obj.get_sub_object_info(j % _obj.get_sub_object_count(), first, cout);
				glDrawArraysInstancedBaseInstance(GL_TRIANGLES, first, cout, 1, j);
			}
		}
	}

	void LoadShaders()
	{
		if (_renderProgram)
			glDeleteProgram(_renderProgram);

		_renderProgram = GetShaderProgram("Shaders/IndirectDraw/IndirectDraw.vert", "Shaders/IndirectDraw/IndirectDraw.frag");

		_uniforms.Time = glGetUniformLocation(_renderProgram, "time");
		_uniforms.ViewMatrix = glGetUniformLocation(_renderProgram, "view_matrix");
		_uniforms.ProjMatrix = glGetUniformLocation(_renderProgram, "proj_matrix");
		_uniforms.ViewProjMatrix = glGetUniformLocation(_renderProgram, "viewproj_matrix");
	}



	void onKey(int key, int action) override
	{
		if (action)
		{
			switch (key)
			{
			case 'P':
				_paused = !_paused;
				break;
			case 'V':
				_vsync = !_vsync;
				setVsync(_vsync);
				break;
			case 'D':
				_mode = Mode(_mode + 1);
				if (_mode > MODE_MAX)
					_mode = MODE_FIRST;
				break;
			case 'R':
				LoadShaders();
				break;
			}
		}
	}

	void shutdown() override
	{
		glDeleteProgram(_renderProgram);

		glDisableVertexAttribArray(0);

		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &_indirectDrawBuffer);
		glDeleteBuffers(1, &_drawIndexBuffer);
	}

private:

	GLuint _renderProgram;
	sb6::object _obj;

	GLuint _indirectDrawBuffer;
	GLuint _drawIndexBuffer;

	struct
	{
		GLint Time;
		GLint ViewMatrix;
		GLint ProjMatrix;
		GLint ViewProjMatrix;
	} _uniforms;

	enum Mode
	{
		MODE_FIRST,
		MODE_MULTIDRAW = 0,
		MODE_SEPARATE_DRAWS,
		MODE_MAX = MODE_SEPARATE_DRAWS
	} _mode;

	bool _paused;
	bool _vsync;

};

DECLARE_MAIN(IndirectDraw);

