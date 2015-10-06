#pragma once

#include <sb6.h>

class GlWindow : public sb6::application
{
public:
	void Render(double currentTime)
	{
		static const GLfloat red[] = {1.0f, 0.0f, 0.0f, 1.0f};
		glClearBufferfv(GL_COLOR, 0, red);
	}
};

DECLARE_MAIN(GlWindow);

