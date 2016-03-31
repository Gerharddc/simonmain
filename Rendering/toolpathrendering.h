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

namespace ToolpathRendering {
    void FreeMemory();
    void Draw();
    void Init();
    void SetToolpath(Toolpath *tp);\
    void ProjMatDirty();
    void SceneMatDirty();
    void SetOpacity(float alpha);
    float GetOpacity();
    void SetColor(glm::vec3 color);
}

#endif // TOOLPATHRENDERER_H
