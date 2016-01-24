#ifndef GRIDRENDERER_H
#define GRIDRENDERER_H

#ifdef GLES
#include <GLES2/gl2.h>
#elif defined(QT_APPLICATION)
#include "loadedgl.h"
#endif

#include "structures.h"
#include "gridgeneration.h"

// Importing this would be a cyclic dependancy
class ComboRendering;

class GridRenderer
{
    friend class ComboRendering;

public:
    GridRenderer(uint xSize, uint ySize, uint zSize, uint interval);
    ~GridRenderer();
    void Draw();
    void Init();

private:
    GLuint mProgram = 0;
    GLsizei mWindowWidth = 0;
    GLsizei mWindowHeight = 0;

    GLint mPositionAttribLocation;

    GLint mModelUniformLocation;
    GLint mProjUniformLocation;

    GLuint mVertexPositionBuffer;

    GridGeneration::Grid *grid;
    unsigned int vertCount;

    // We need flags to determine when matrices have changed as
    // to be able to give new ones to opengl
    bool dirtyProjMat = true;
    bool dirtySceneMat = true;
};

#endif // GRIDRENDERER_H
