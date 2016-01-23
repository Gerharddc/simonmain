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
    GLint mProjUniformLocation;
    GLint mNormUniformLocation;

    GLuint mVertexPositionBuffer;
    GLuint mVertexNormalBuffer;

    int mDrawCount;
    Mesh *mesh;

    float x, y = 0.0f;

    float aimX = 50.0f;
    float aimY = 50.0f;

    float centreX = 0.0f;
    float centreY = 0.0f;

    float zoom = 3.0f;
};

#endif // STLRENDERER_H
