#ifndef COMBORENDERING_H
#define COMBORENDERING_H

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <set>

#include "stlrendering.h"
#include "toolpathrendering.h"
#include "gridrendering.h"
#include "structures.h"

namespace ComboRendering
{
    void FreeMemory();

    void SetViewSize(float width, float height);
    void Init();
    void Draw();
    void ApplyRot(float x, float y);
    void Move(float x, float y);
    void Zoom(float scale);
    void ResetView();
    void LoadMesh(const char* path);
    void RemoveMesh(Mesh *mesh);
    std::string SaveMeshes(std::string fileName);
    std::string SliceMeshes();

    void TestMouseIntersection(float x, float y, bool &needUpdate);

    const std::set<Mesh*> &getSelectedMeshes();

    const glm::mat4 &getSceneTrans();
    const glm::mat4 &getSceneProj();
    int getMeshCount();
}

#endif // COMBORENDERING_H
