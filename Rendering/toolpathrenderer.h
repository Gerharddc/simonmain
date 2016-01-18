#ifndef TOOLPATHRENDERER_H
#define TOOLPATHRENDERER_H

#ifdef GLES
#include <GLES2/gl2.h>
#elif defined(QT_APPLICATION)
#include "loadedgl.h"
#endif

#include "structures.h"

class ToolpathRenderer
{
public:
    ToolpathRenderer(Toolpath* _path);
    ~ToolpathRenderer();
    void Draw();
    void UpdateWindowSize(GLsizei width, GLsizei height);
    void Init();

private:
    GLuint mProgram = 0;
    GLsizei mWindowWidth = 0;
    GLsizei mWindowHeight = 0;

    GLint mPositionAttribLocation;

    GLint mModelUniformLocation;
    GLint mViewUniformLocation;
    GLint mProjUniformLocation;

    GLuint mVertexPositionBuffer;

    int mDrawCount;
    Toolpath *path;

    float xAim = 0;
    float yAim = 0;
};

#endif // TOOLPATHRENDERER_H
