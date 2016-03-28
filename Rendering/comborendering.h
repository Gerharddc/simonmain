#ifndef COMBORENDERING_H
#define COMBORENDERING_H

#include <glm/glm.hpp>

#include "stlrenderer.h"
#include "toolpathrenderer.h"
#include "gridrenderer.h"
#include "structures.h"

class ComboRendering
{
    friend class STLRenderer;
    friend class ToolpathRenderer;
    friend class GridRenderer;

private:
    static glm::mat4 sceneTrans, sceneProj;
    static float viewWidth, viewHeight;
    static float centreX, centreY;
    static float aimX, aimY;
    static float sizeX, sizeY, sizeZ;
    static float zoom;

    static void UpdateProjection();
    static void UpdateTransform();
    static void RecalculateCentre();

    static glm::mat4 rotOrg, rotOrgInv;
    static glm::quat sceneRot;

    static STLRenderer stlRen;
    static ToolpathRenderer tpRen;
    static GridRenderer gridRen;

    Mesh *stlMesh = nullptr;
    Toolpath *gcodePath = nullptr;

    static float meshOpacity;
    static float tpOpacity;

public:
    ComboRendering();
    ~ComboRendering();

    void SetViewSize(float width, float height);
    void Init();
    void Draw();
    void ApplyRot(float x, float y);
    void Move(float x, float y);
    void Zoom(float scale);
    void ResetView();

    void SetMeshOpacity(float opacity);
    void SetTpOpacity(float opacity);
    float MeshOpacity();
    float TpOpacity();
};

#endif // COMBORENDERING_H
