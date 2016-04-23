#ifndef TOOLPATHRENDERER_H
#define TOOLPATHRENDERER_H

#ifdef GLES
#include <GLES2/gl2.h>
#elif defined(QT_APPLICATION)
#include "loadedgl.h"
#endif

#include <array>
#include "structures.h"

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
    bool ToolpathLoaded();
}

#endif // TOOLPATHRENDERER_H
