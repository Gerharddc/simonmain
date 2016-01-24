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
#ifdef REDUNDANT
    static float sceneXRot, sceneYRot, sceneZRot;
#endif

    static void UpdateProjection();
    static void UpdateTransform();

    bool movedSinceRot = true;
    glm::mat4 rotOrg, rotOrgInv;

    //STLRenderer stlRen;
    //ToolpathRenderer tpRen;
    static GridRenderer gridRen;

    //Mesh *stlMesh = nullptr;
    Mesh *mesh;

public:
    ComboRendering();
    ~ComboRendering();

    void SetViewSize(float width, float height);
    void Init();
    void Draw();
    void ApplyRot(float x, float y);
    void Move(float x, float y);
};

#endif // COMBORENDERING_H