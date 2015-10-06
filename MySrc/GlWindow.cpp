#include <sb6.h>
#include <cmath>
#include <iostream>
#include <sstream>

void Log(float arg)
{
	std::wstringstream wss;
	wss << arg << std::endl;
	OutputDebugString(wss.str().c_str());
}

class GlWindow : public sb6::application
{
public:
	void render(double currentTime)
	{
		const GLfloat red[] = { sin(currentTime)*0.5f + 0.5f, cos(currentTime)*0.5f + 0.5f, 0.0f, 1.0f };
		float val = red[0];
		Log(val);
		glClearBufferfv(GL_COLOR, 0, red);
	}
};

DECLARE_MAIN(GlWindow);