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
    GLint mProjUniformLocation;

    GLuint mVertexPositionBuffer;

    int mDrawCount;
    GridGeneration::Grid *grid;
    unsigned int vertCount;

    // These represent the position in scene space that should be in the centre of the viewport
    float aimX = 50.0f;
    float aimY = 50.0f;

    // These are the centre position of the display area in pixels
    // and will e calculated when the viewport size is set
    float centreX = 0;
    float centreY = 0;

    float zoom = 3.0f;
};

#endif // GRIDRENDERER_H
