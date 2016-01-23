#ifndef GRIDRENDERER_H
#define GRIDRENDERER_H

#ifdef GLES
#include <GLES2/gl2.h>
#elif defined(QT_APPLICATION)
#include "loadedgl.h"
#endif

#include "structures.h"
#include "gridgeneration.h"

#define uint unsigned int

class GridRenderer
{
public:
    GridRenderer(uint xSize, uint ySize, uint zSize, uint interval);
    ~GridRenderer();
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
    GridGeneration::Grid *grid;
    unsigned int vertCount;

    float xAim = 0;
    float yAim = 0;
};

#endif // GRIDRENDERER_H
