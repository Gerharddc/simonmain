#include "comborendering.h"

#include <QDebug>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "stlimporting.h"
#include "gcodeimporting.h"
#include "Misc/globalsettings.h"

// Init
const float DefaultZoom = 3.0f;

float ComboRendering::viewWidth = 0.0f;
float ComboRendering::viewHeight = 0.0f;
float ComboRendering::centreX = 0.0f;
float ComboRendering::centreY = 0.0f;
float ComboRendering::aimX = 50.0f;
float ComboRendering::aimY = 50.0f;
float ComboRendering::zoom = DefaultZoom;
glm::mat4 ComboRendering::sceneTrans = glm::mat4();
glm::mat4 ComboRendering::sceneProj = glm::mat4();
GridRenderer ComboRendering::gridRen = GridRenderer(100, 100, 100, 10);
STLRenderer ComboRendering::stlRen = STLRenderer();
ToolpathRenderer ComboRendering::tpRen = ToolpathRenderer();
glm::mat4 ComboRendering::rotOrg = glm::mat4();
glm::mat4 ComboRendering::rotOrgInv = glm::mat4();
glm::quat ComboRendering::sceneRot = glm::quat();
float ComboRendering::meshOpacity = 1.0f;

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

    // Reset the scene view looking at the centre of the box from a height above the origin
    RecalculateCentre();
    sceneTrans = rotOrg;
    sceneRot = glm::angleAxis(glm::radians(180.0f + 45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::quat inv = sceneRot;
    inv.w *= -1; // Invert the angle
    glm::vec4 xDir(1.0f, 0.0f, 0.0f, 1.0f);
    xDir = inv * xDir;
    sceneRot *= glm::angleAxis(glm::radians(45.0f), glm::vec3(xDir.x, xDir.y, 0.0f));
    sceneTrans *= glm::mat4_cast(sceneRot);
    sceneTrans *= rotOrgInv;
}

void ComboRendering::UpdateProjection()
{
    // Calculate an orthographic projection that centres the view at the aiming position and applies the zoom
    float left = aimX - (centreX / zoom);
    float right = aimX + (centreX / zoom);
    float bottom = aimY - (centreY / zoom);
    float top = aimY + (centreY / zoom);

    // With the orthographic system we need a negative and positive clip plane of enough distance
    sceneProj = glm::ortho(left, right, bottom, top, -GlobalSettings::BedHeight.Get() * 10, GlobalSettings::BedHeight.Get() * 10);

    // Flag the renderers to update their proj matrices
    gridRen.ProjMatDirty();
    stlRen.ProjMatDirty();
    tpRen.ProjMatDirty();
}

void ComboRendering::RecalculateCentre()
{
    // We need to translate the centre of the box to the origin for rotation
    float midX = GlobalSettings::BedWidth.Get() / 2.0f;
    float midY = GlobalSettings::BedLength.Get() / 2.0f;

    rotOrg = glm::translate(glm::mat4(), glm::vec3(midX, midY, 0.0f));
    rotOrgInv = glm::translate(glm::mat4(), glm::vec3(-midX, -midY, 0.0f));
}

void ComboRendering::ApplyRot(float x, float y)
{
    // We need to transform the rotation axis into screen space by applying the inverse of the rotation
    // currently being applied to our scene

    // We also need to scale the roation to the screen size and aspect ratio
    // TODO: add dpi scaling
    // For some reason the x rot is in the inverse direction of the mouse movement
    // TODO: 3.0 ?
    float yAng = (x / GlobalSettings::BedWidth.Get() * 1.1) / 3.0;
    float xAng = -(y / GlobalSettings::BedLength.Get() * (1.1 * GlobalSettings::BedLength.Get() / GlobalSettings::BedWidth.Get())) / 3.0;
    glm::vec2 axis = glm::vec2(xAng, yAng);
    auto l = glm::length(axis); // doesnt work
    if (l == 0) // This is important to avoid normaliztion disasters ahead
        return;

    axis = glm::normalize(axis);
    glm::vec4 dir = glm::vec4(axis.x, axis.y, 0.0f, 0.0f);
    glm::quat inv = sceneRot;
    inv.w *= -1;
    dir = inv * dir;
    dir = glm::normalize(dir);

    // Now we need to rotate accordingly around the calculated centre
    sceneTrans = rotOrg;
    sceneRot *= glm::angleAxis(l, glm::vec3(dir.x, dir.y, dir.z));
    sceneTrans *= glm::mat4_cast(sceneRot);
    sceneTrans *= rotOrgInv;

    // Flag the renderers to update their scene matrices
    gridRen.SceneMatDirty();
    stlRen.SceneMatDirty();
    tpRen.SceneMatDirty();
}

void ComboRendering::Move(float x, float y)
{
    // The direction of mouse movement is inverse of the view movement
    // We need to scale the move distance to the size of the viewport
    // We want a constant move distance for a viewport length and take
    // aspect ration into account
    // TODO: add dpi support
    aimX -= (x / GlobalSettings::BedWidth.Get() * 65) / zoom;
    aimY -= (y / GlobalSettings::BedLength.Get() * (65 * GlobalSettings::BedLength.Get() / GlobalSettings::BedWidth.Get())) / zoom;

    UpdateProjection();
}

void ComboRendering::Zoom(float scale)
{
    zoom *= scale;
    UpdateProjection();
}

void ComboRendering::ResetView()
{
    zoom = DefaultZoom;

    SetViewSize(viewWidth, viewHeight);
    aimX = GlobalSettings::BedWidth.Get() / 2.0f;
    aimY = GlobalSettings::BedLength.Get() / 2.0f;

    gridRen.SceneMatDirty();
    stlRen.SceneMatDirty();
    tpRen.SceneMatDirty();
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

    gridRen.Draw();

    if (meshOpacity != 0.0f)
        stlRen.Draw();

    if (TpOpacity() != 0.0f)
        tpRen.Draw();
}

// TODO: maybe working with this local copy is dangerous...

float ComboRendering::MeshOpacity()
{
    return meshOpacity;
}

float ComboRendering::TpOpacity()
{
    return tpRen.GetOpacity();
}

void ComboRendering::SetMeshOpacity(float opacity)
{
    meshOpacity = opacity;
    stlRen.ColorAll(opacity);
}

void ComboRendering::SetTpOpacity(float opacity)
{
    tpRen.SetOpacity(opacity);
}
