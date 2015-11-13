#version 430 core

out vec4 col;
subroutine vec4 ColorPicker();

layout (index = 0)
subroutine (ColorPicker)
vec4 GreenColor()
{
	return vec4(0.0, 1.0, 0.0, 0.0);
}

layout (index = 1)
subroutine (ColorPicker)
vec4 BlueColor()
{
	return vec4(0.0, 0.0, 1.0, 0.0);
}

subroutine uniform ColorPicker currentColor;

void main(void)
{
	col = currentColor();
}