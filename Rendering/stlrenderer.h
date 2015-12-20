#ifndef STLRENDERER_H
#define STLRENDERER_H

#ifdef GLES
#include <GLES2/gl2.h>
#elif defined(QT_APPLICATION)
#include "loadedgl.h"
#endif

class STLRenderer
{
public:
    STLRenderer();
    ~STLRenderer();
    void Draw();
    void UpdateWindowSize(GLsizei width, GLsizei height);
    void Init();

private:
    GLuint mProgram = 0;
    GLsizei mWindowWidth = 0;
    GLsizei mWindowHeight = 0;

    GLint mPositionAttribLocation;
    GLint mColorAttribLocation;

    GLint mModelUniformLocation;
    GLint mViewUniformLocation;
    GLint mProjUniformLocation;

    GLuint mVertexPositionBuffer;
    GLuint mVertexColorBuffer;
    GLuint mIndexBuffer;

    int mDrawCount;
};

#endif // STLRENDERER_H
