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
    ToolpathRenderer();
    ~ToolpathRenderer();
    void Draw();
    void Init();
    void SetToolpath(Toolpath *tp);\
    void ProjMatDirty();
    void SceneMatDirty();

private:
    GLuint mProgram = 0;
    GLsizei mWindowWidth = 0;
    GLsizei mWindowHeight = 0;

    //GLint mPositionAttribLocation;

    GLint mCurPosAttribLocation;
    GLint mNextPosAttribLocation;
    GLint mPrevPosAttribLocation;
    GLint mSideAttribLocation;
    GLint mZAttribLocation;

    GLint mModelUniformLocation;
    //GLint mViewUniformLocation;
    GLint mRadiusUniformLocation;
    GLint mProjUniformLocation;

    //GLuint mVertexPositionBuffer = 0;

    GLuint mCurPosBuffer = 0;
    GLuint mNextPosBuffer = 0;
    GLuint mPrevPosBuffer = 0;
    GLuint mSideBuffer = 0;
    GLuint mZBuffer = 0;

    inline void LoadPath();
    Toolpath *path;

    // We need flags to determine when matrices have changed as
    // to be able to give new ones to opengl
    bool dirtyProjMat = true;
    bool dirtySceneMat = true;
    bool dirtyPath = false;
};

#endif // TOOLPATHRENDERER_H
