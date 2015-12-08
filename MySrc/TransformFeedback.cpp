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

DECLARE_MAIN(TransformFeedback);

