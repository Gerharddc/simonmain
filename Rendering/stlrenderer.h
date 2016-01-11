#ifndef STLRENDERER_H
#define STLRENDERER_H

#ifdef GLES
#include <GLES2/gl2.h>
#elif defined(QT_APPLICATION)
#include "loadedgl.h"
#endif

#include "structures.h"

class STLRenderer
{
public:
    STLRenderer(Mesh* _mesh);
    ~STLRenderer();
    void Draw();
    void UpdateWindowSize(GLsizei width, GLsizei height);
    void Init();

private:
    GLuint mProgram = 0;
    GLsizei mWindowWidth = 0;
    GLsizei mWindowHeight = 0;

    GLint mPositionAttribLocation;
    GLint mNormalAttribLocation;

    GLint mModelUniformLocation;
    GLint mViewUniformLocation;
    GLint mProjUniformLocation;
    GLint mNormUniformLocation;

    GLuint mVertexPositionBuffer;
    GLuint mIndexBuffer;
    GLuint mVertexNormalBuffer;

    int mDrawCount;
    Mesh *mesh;

    float x = 0;
    float y = 0;
};

#endif // STLRENDERER_H
