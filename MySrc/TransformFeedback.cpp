#include <sb6.h>
#include <vmath.h>
#include <shader.h>

using vmath::vec3;
using vmath::vec4;
using vmath::ivec4;

class TransformFeedback : public sb6::application
{
public:
	enum BufferType
	{
		PositionA,
		PositionB,
		VelocityA,
		VelocityB,
		Connection
	};

	enum
	{
		PointsX = 50,
		PointsY = 50,
		PointsTotal = PointsX * PointsY,
		ConnectionsTotal = (PointsX - 1) * PointsY + (PointsY - 1) * PointsX
	};

	TransformFeedback() : _updateProgram(0), _renderProgram(0), _iterationIndex(0), _drawPoints(true), _drawLines(true), _iterationPerFrame(16)
	{
	}

	void LoadShaders()
	{
		GLuint vs;
		GLuint fs;
		char buffer[1024];

		vs = sb6::shader::load("Shaders/TransformFeedback/FeedbackUpdate.vert", GL_VERTEX_SHADER);
		if (_updateProgram)
			glDeleteProgram(_updateProgram);
		_updateProgram = glCreateProgram();
		glAttachShader(_updateProgram, vs);

		static const char* varyings[] =
		{
			"tf_position_mass",
			"tf_velocity"
		};
		glTransformFeedbackVaryings(_updateProgram, 2, varyings, GL_SEPARATE_ATTRIBS);

		glLinkProgram(_updateProgram);

		glGetShaderInfoLog(vs, 1024, nullptr, buffer);
		glGetProgramInfoLog(_updateProgram, 1024, nullptr, buffer);
		glDeleteShader(vs);

		vs = sb6::shader::load("Shaders/TransformFeedback/FeedbackRender.vert", GL_VERTEX_SHADER);
		fs = sb6::shader::load("Shaders/TransformFeedback/FeedbackRender.frag", GL_FRAGMENT_SHADER);

		if (_renderProgram)
			glDeleteProgram(_renderProgram);
		_renderProgram = glCreateProgram();
		glAttachShader(_renderProgram, vs);
		glAttachShader(_renderProgram, fs);

		glLinkProgram(_renderProgram);
	}

	void startup() override
	{
		int i, j;
		LoadShaders();
		vec4* initialPositions = new vec4[PointsTotal];
		vec3* initialVelocities = new vec3[PointsTotal];
		ivec4* connectionsVectors = new ivec4[PointsTotal];

		int n = 0;
		for (j = 0; j < PointsY; j++)
		{
			float fj = (float)j / (float)PointsY;
			for (i = 0; i < PointsX; i++)
			{
				float fi = (float)i / (float)PointsX;
				initialPositions[n] = vec4(
					(fi - 0.5f) * (float)PointsX,
					(fj - 0.5f) * (float)PointsY,
					0.6f * sinf(fi) * cosf(fj),
					1.0f
				);
				initialVelocities[n] = vec3(0.0f);
				connectionsVectors[n] = ivec4(-1);

				if (j != (PointsY - 1))
				{
					if (i != 0)
						connectionsVectors[n][0] = n - 1;
					if (j != 0)
						connectionsVectors[n][1] = n - PointsX;
					if (i != PointsX - 1)
						connectionsVectors[n][2] = n + 1;
					if (j != PointsY - 1)
						connectionsVectors[n][3] = n + PointsX;
				}
				n++;
			}
		}
		glGenVertexArrays(2, _vao);
		glGenBuffers(5, _vbo);

		for (i = 0; i < 2; i++)
		{
			glBindVertexArray(_vao[i]);

			glBindBuffer(GL_ARRAY_BUFFER, _vbo[PositionA + i]);
			glBufferData(GL_ARRAY_BUFFER, PointsTotal * sizeof(vec4), initialPositions, GL_DYNAMIC_COPY);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, _vbo[VelocityA + i]);
			glBufferData(GL_ARRAY_BUFFER, PointsTotal * sizeof(vec3), initialVelocities, GL_DYNAMIC_COPY);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, _vbo[Connection]);
			glBufferData(GL_ARRAY_BUFFER, PointsTotal * sizeof(ivec4), connectionsVectors, GL_STATIC_DRAW);
			glVertexAttribIPointer(2, 4, GL_INT, 0, nullptr);
			glEnableVertexAttribArray(2);
		}

		delete[] connectionsVectors;
		delete[] initialVelocities;
		delete[] initialPositions;

		glGenTextures(2, _posTbo);
		glBindTexture(GL_TEXTURE_BUFFER, _posTbo[0]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, _vbo[PositionA]);
		glBindTexture(GL_TEXTURE_BUFFER, _posTbo[1]);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, _vbo[PositionB]);

		int lines = (PointsX - 1) * PointsY + (PointsY - 1) * PointsX;

		glGenBuffers(1, &_indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, lines * 2 * sizeof(int), nullptr, GL_STATIC_DRAW);

		int* e = (int*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, lines * 2 * sizeof(int), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		for (j = 0; j < PointsY; j++)
		{
			for (i = 0; i < PointsX - 1; i++)
			{
				*e++ = i + j * PointsX;
				*e++ = 1 + i + j * PointsX;
			}
		}
		for (i = 0; i < PointsX; i++)
		{
			for (j = 0; j < PointsY - 1; j++)
			{
				*e++ = i + j * PointsX;
				*e++ = PointsX + i + j * PointsX;
			}
		}
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	}

	void shutdown()
	{
		glDeleteProgram(_updateProgram);
		glDeleteProgram(_renderProgram);
		glDeleteBuffers(5, _vbo);
		glDeleteVertexArrays(2, _vao);
	}

	void render(double t)
	{
		int i;
		glUseProgram(_updateProgram);
		glEnable(GL_RASTERIZER_DISCARD);

		for (i = _iterationPerFrame; i != 0; --i)
		{
			glBindVertexArray(_vao[_iterationIndex & 1]);
			glBindTexture(GL_TEXTURE_BUFFER, _posTbo[_iterationIndex & 1]);
			_iterationIndex++;
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _vbo[PositionA + (_iterationIndex & 1)]);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, _vbo[VelocityA + (_iterationIndex & 1)]);
			glBeginTransformFeedback(GL_POINTS);
			glDrawArrays(GL_POINTS, 0, PointsTotal);
			glEndTransformFeedback();
		}
		glDisable(GL_RASTERIZER_DISCARD);

		static const GLfloat black[] = {0.0f, 0.0f, 0.0f, 0.0f};
		glViewport(0, 0, info.windowWidth, info.windowHeight);
		glClearBufferfv(GL_COLOR, 0, black);
		glUseProgram(_renderProgram);
		if (_drawPoints)
		{
			glPointSize(4.0f);
			glDrawArrays(GL_POINTS, 0, PointsTotal);
		}
		if (_drawLines)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
			glDrawElements(GL_LINES, ConnectionsTotal * 2, GL_UNSIGNED_INT, nullptr);
		}
	}

	void onKey(int key, int action)
	{
		if (action)
		{
			switch(key)
			{
			case 'R': LoadShaders();
				break;
			case 'L': _drawLines = !_drawLines;
				break;
			case 'P': _drawPoints = !_drawPoints;
				break;
			case GLFW_KEY_KP_ADD: _iterationPerFrame++;
				break;
			case GLFW_KEY_KP_SUBTRACT: _iterationPerFrame--;
				break;
			}
		}
	}

private:
	GLuint _vao[2];
	GLuint _vbo[5];
	GLuint _indexBuffer;
	GLuint _posTbo[2];
	GLuint _updateProgram;
	GLuint _renderProgram;
	GLuint _iterationIndex;;

	bool _drawPoints;
	bool _drawLines;
	int _iterationPerFrame;
};

//DECLARE_MAIN(TransformFeedback);

