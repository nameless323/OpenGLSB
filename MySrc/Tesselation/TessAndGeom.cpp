#include <sb6.h>

#include "../ShaderProgram.h"

namespace OpenGlSB
{
class TessAndGeom : public sb6::application
{
public:
    void startup()
    {
        //		_renderingProgram.CreateAndLinkProgram("Shaders/Common/Default.vert", "Shaders/Common/Default.frag");
        _renderingProgram.CreateProgram();
        _renderingProgram.AttachShader("Shaders/Tesselation/TessAndGeom/TessAndGeom.vert");
        _renderingProgram.AttachShader("Shaders/Tesselation/TessAndGeom/TessAndGeom.tesc");
        _renderingProgram.AttachShader("Shaders/Tesselation/TessAndGeom/TessAndGeom.tese");
        _renderingProgram.AttachShader("Shaders/Tesselation/TessAndGeom/TessAndGeom.geom");
        _renderingProgram.AttachShader("Shaders/Tesselation/TessAndGeom/TessAndGeom.frag");
        _renderingProgram.Link();
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
    }

    void onResize(int w, int h)
    {
        application::onResize(w, h);
    }

    void render(double currentTime)
    {
        _renderingProgram.Use();
        const GLfloat bckColor[] = {0.0f, 0.25f, 0.0f, 1.0f};
        const GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, bckColor);
        glClearBufferfv(GL_DEPTH, 0, white);
        //		glClear(GL_DEPTH_BUFFER_BIT);

        glPointSize(5.0f);
        glDrawArrays(GL_PATCHES, 0, 3);
    }

    void shutdown()
    {
        _renderingProgram.DeleteProgram();

        glDisableVertexAttribArray(0);

        glBindVertexArray(0);
        glDeleteVertexArrays(1, &_vao);
    }

private:
    ShaderProgram _renderingProgram;
    GLuint _vao;
};
}
//DECLARE_MAIN(TessAndGeom);
