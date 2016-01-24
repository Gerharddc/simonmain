#include "comborendering.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "stlimporting.h"
#include "gcodeimporting.h"

// Init
float ComboRendering::viewWidth = 0.0f;
float ComboRendering::viewHeight = 0.0f;
float ComboRendering::centreX = 0.0f;
float ComboRendering::centreY = 0.0f;
float ComboRendering::aimX = 50.0f;
float ComboRendering::aimY = 50.0f;
float ComboRendering::sizeX = 100.0f;
float ComboRendering::sizeY = 100.0f;
float ComboRendering::sizeZ = 100.0f;
#ifdef REDUNDANT
float ComboRendering::sceneXRot = 0.0f;
float ComboRendering::sceneYRot = 0.0f;
float ComboRendering::sceneZRot = 0.0f;
#endif
float ComboRendering::zoom = 3.0f;
glm::mat4 ComboRendering::sceneTrans = glm::mat4();
glm::mat4 ComboRendering::sceneProj = glm::mat4();
GridRenderer ComboRendering::gridRen = GridRenderer(100, 100, 100, 10);
STLRenderer ComboRendering::stlRen = STLRenderer();
ToolpathRenderer ComboRendering::tpRen = ToolpathRenderer();

ComboRendering::ComboRendering()
{
    stlMesh = STLImporting::ImportSTL("bin.stl");
    stlRen.AddMesh(stlMesh);

    gcodePath = GCodeImporting::ImportGCode("test.gcode");
    tpRen.SetToolpath(gcodePath);
}

ComboRendering::~ComboRendering()
{
    if (stlMesh != nullptr)
    {
        delete stlMesh;
        stlMesh = nullptr;
    }

    if (gcodePath != nullptr)
    {
        delete gcodePath;
        gcodePath = nullptr;
    }
}

void ComboRendering::SetViewSize(float width, float height)
{
    // Update the viewport variables
    viewWidth = width;
    viewHeight = height;
    centreX = width / 2.0f;
    centreY = height / 2.0f;

    // Update the projection
    UpdateProjection();
}

void ComboRendering::UpdateProjection()
{
    // Calculate an orthographic projection that centres the view at the aiming position and applies the zoom
    float left = aimX - (centreX / zoom);
    float right = aimX + (centreX / zoom);
    float bottom = aimY - (centreY / zoom);
    float top = aimY + (centreY / zoom);

    // With the orthographic system we need a negative and positive clip plane of enough distance
    sceneProj = glm::ortho(left, right, bottom, top, -sizeZ * 2, sizeZ * 2);

    // Flag the renderers to update their proj matrices
    gridRen.dirtyProjMat = true;
    stlRen.ProjMatDirty();
    tpRen.ProjMatDirty();

}

const glm::vec3 yVec(0.0f, 1.0f, 0.0f);
const glm::vec3 xVec(1.0f, 0.0f, 0.0f);

void ComboRendering::ApplyRot(float x, float y)
{
    // We want to rotate around whatever point is in the centre of the screen
    // if we have panned then we need to recalculate the point underneath the
    // centre of the screen
    if (movedSinceRot)
    {
        movedSinceRot = false;

        // Project the centre screen pos into scene space on z = 0 (floor)
        glm::mat4 inv = glm::inverse(sceneProj * sceneTrans);
        // The x and y should be between -1 and 1 thus 0
        glm::vec4 centre(0.0f, 0.0f, 1.0f, 1.0f); // Not sure if z and w are needed...
        centre = inv * centre; // Order is important here
        rotOrg = glm::translate(glm::mat4(), glm::vec3(centre.x, centre.y, 0.0f));
        rotOrgInv = glm::translate(glm::mat4(), glm::vec3(-centre.x, -centre.y, 0.0f));
    }

    // Now we need to rotate accordingly around the calculated centre
    sceneTrans *= rotOrg;
    sceneTrans = glm::rotate(sceneTrans, y, yVec);
    sceneTrans = glm::rotate(sceneTrans, x, xVec);
    sceneTrans *= rotOrgInv;

    // Flag the renderers to update their scene matrices
    gridRen.dirtySceneMat = true;
    stlRen.SceneMatDirty();
    tpRen.SceneMatDirty();
}

void ComboRendering::Move(float x, float y)
{
    aimX += x;
    aimY += y;

    UpdateProjection();

    if (!movedSinceRot)
        movedSinceRot = true;
}

void ComboRendering::Init()
{
    gridRen.Init();
    stlRen.Init();
    tpRen.Init();
}

void ComboRendering::Draw()
{
    glViewport(0, 0, viewWidth, viewHeight);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ApplyRot(0.01f, 0.01f);

    gridRen.Draw();
    stlRen.Draw();
    tpRen.Draw();
}

#ifdef REDUNDANT
void ComboRendering::UpdateTransform()
{
    // Use quaternions to avoid gimbal lock
    glm::quat xRot = glm::angleAxis(sceneXRot, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat yRot = glm::angleAxis(sceneYRot, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat zRot = glm::angleAxis(sceneZRot, glm::vec3(0.0f, 0.0f, 1.0f));

    // Combine them and caclualte the matrix
    glm::quat rot = xRot * yRot * zRot;
    sceneTrans = glm::mat4_cast(rot);

    // Flag the renderers to update their trans matrices
    // TODO
}
#endif
