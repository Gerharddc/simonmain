#ifndef TOOLPATHRENDERER_H
#define TOOLPATHRENDERER_H

#ifdef GLES
#include <GLES2/gl2.h>
#elif defined(QT_APPLICATION)
#include "loadedgl.h"
#endif

#include <array>
#include "structures.h"

struct GroupGLData
{
    ~GroupGLData();

    GLuint mCurPosBuffer = 0;
    GLuint mNextPosBuffer = 0;
    GLuint mPrevPosBuffer = 0;
    GLuint mSideBuffer = 0;

    uint16_t *indices;
    short idxCount = 0;
};

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

    void SetOpacity(float alpha);
    float GetOpacity();
    void SetColor(glm::vec3 color);

private:
    GLuint mProgram = 0;
    GLsizei mWindowWidth = 0;
    GLsizei mWindowHeight = 0;

    GLint mCurPosAttribLocation;
    GLint mNextPosAttribLocation;
    GLint mPrevPosAttribLocation;
    GLint mSideAttribLocation;

    GLint mModelUniformLocation;
    GLint mRadiusUniformLocation;
    GLint mProjUniformLocation;
    GLint mColorUniformLocation;

    GroupGLData *groupDatas = nullptr;
    std::size_t groupCount = 0;

    inline void LoadPath();
    Toolpath *path;

    // We need flags to determine when matrices have changed as
    // to be able to give new ones to opengl
    bool dirtyProjMat = true;
    bool dirtySceneMat = true;
    bool dirtyPath = false;
    bool dirtyColor = true;

    glm::vec3 _color = glm::vec3(0.2f, 0.2f, 0.8f);
    float opacity = 1.0f;
};

#endif // TOOLPATHRENDERER_H
