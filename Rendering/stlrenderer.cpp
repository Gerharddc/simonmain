#include "stlrenderer.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glhelper.h"
#include "mathhelper.h"
#include "stlimporting.h"
#include "comborendering.h"
#include "Misc/globalsettings.h"

STLRenderer::STLRenderer()
{

}

// This updates the Mesh in normal memory for saving but does not
// affect the mesh loaded in GPU memory
void UpdateMesh(Mesh *mesh, MeshGroupData *mg)
{
    //Apply the matrix to the mesh
    for (std::size_t i = 0; i < mesh->vertexCount; i++)
    {
        // TODO: optomize...
        auto idx = i * 3;
        glm::vec4 v = glm::vec4(mesh->vertexFloats[idx + 0], mesh->vertexFloats[idx + 1], mesh->vertexFloats[idx + 2], 1.0f);
        v = mg->gpuMat * glm::inverse(mg->meshMat) * v;
        mesh->vertexFloats[idx + 0] = v.x;
        mesh->vertexFloats[idx + 1] = v.y;
        mesh->vertexFloats[idx + 2] = v.z;
    }

    // Update the parameters
    mg->rotOnMesh = mg->rotOnMat;
    mg->scaleOnMesh = mg->scaleOnMat;
    mg->moveOnMesh = mg->moveOnMat + mg->meshCentre;
    mg->meshMat = mg->gpuMat;
}

void MeshGroupData::Destroy()
{
    // A destroy function is used instead of a destructor because
    // the std::vector will be constantly moving this thing around
    // and it's way too much effort to use move constructors for this
    // limited scope class

    // Delete the buffers

    if (mVertexPositionBuffer != 0)
    {
        glDeleteBuffers(1, &mVertexPositionBuffer);
        mVertexPositionBuffer = 0;
    }

    if (mVertexNormalBuffer != 0)
    {
        glDeleteBuffers(1, &mVertexNormalBuffer);
        mVertexNormalBuffer = 0;
    }
}

STLRenderer::~STLRenderer()
{
    // Delete all the meshes on the heap
    for (auto &gPair : meshGroups)
        gPair.second.Destroy();

    // Delete the program
    if (mProgram != 0)
    {
        glDeleteProgram(mProgram);
        mProgram = 0;
    }
}

void STLRenderer::ProjMatDirty()
{
    dirtyProjMat = true;
}

void STLRenderer::SceneMatDirty()
{
    dirtySceneMat = true;
}

void STLRenderer::AddMesh(Mesh *mesh)
{
    // Add a new mesh data group to the vector that contains all the metadata and helpers
    MeshGroupData mg;
    mg.meshDirty = true;
    meshGroups.emplace(mesh, mg);

    // Signal the global dirty mesh flag
    dirtyMesh = true;
}

void STLRenderer::RemoveMesh(Mesh *mesh)
{
    // Destroy the mesh and then remove it
    meshGroups[mesh].Destroy();
    meshGroups.erase(mesh);
}

// TODO: these transofrmations hould probably be applied in a bg thread

// Update the meshdatagroup matrix
inline void UpdateTempMat(MeshGroupData &mg)
{
    // TODO: optomize ?
    mg.gpuMat = glm::translate(glm::mat4(1.0f), mg.moveOnMat);
    mg.gpuMat = glm::scale(mg.gpuMat, glm::vec3(mg.scaleOnMat));
    mg.gpuMat = mg.gpuMat * glm::mat4(glm::quat(mg.rotOnMat));
    mg.gpuMat = glm::translate(mg.gpuMat, mg.meshCentre);

    mg.sceneMatsDirty = true;
}

// This method applies an absolute scale to the original mesh
void STLRenderer::ScaleMesh(Mesh *mesh, float absScale)
{
    MeshGroupData &mg = meshGroups[mesh];

    mg.scaleOnMat = absScale;

    UpdateTempMat(mg);
}

// This method centres the mesh around the current coordinates
void STLRenderer::CentreMesh(Mesh *mesh, float absX, float absY)
{
    MeshGroupData &mg = meshGroups[mesh];

    mg.moveOnMat.x = absX;
    mg.moveOnMat.y = absY;

    UpdateTempMat(mg);
}

// This method places the mesh an absolute height above the bed
void STLRenderer::LiftMesh(Mesh *mesh, float absZ)
{
    MeshGroupData &mg = meshGroups[mesh];

    mg.moveOnMat.z = absZ;

    UpdateTempMat(mg);
}

// This method applies an absolute rotation to the original mesh
void STLRenderer::RotateMesh(Mesh *mesh, float absX, float absY, float absZ)
{
    MeshGroupData &mg = meshGroups[mesh];

    mg.rotOnMat = glm::vec3(absX, absY, absZ);

    UpdateTempMat(mg);
}

void STLRenderer::LoadMesh(MeshGroupData &mg, Mesh *mesh)
{
    glGenBuffers(1, &mg.mVertexPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mg.mVertexPositionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->trigCount * 9, mesh->getFlatVerts(), GL_STATIC_DRAW);
    mesh->dumpFlatVerts();

    glGenBuffers(1, &mg.mVertexNormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mg.mVertexNormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->trigCount * 9, mesh->getFlatNorms(), GL_STATIC_DRAW);
    mesh->dumpFlatNorms();

    float x = (mesh->MaxVec.x + mesh->MinVec.x) / 2.0f;
    float y = (mesh->MaxVec.y + mesh->MinVec.y) / 2.0f;
    mg.meshCentre = glm::vec3(GlobalSettings::BedWidth.Get() / 2 - x, GlobalSettings::BedLength.Get() / 2 - y, -mesh->MinVec.z);

    // Create a bounding sphere around the centre of the mesh using the average distance as radius
    mg.bSphereCentre = mesh->Centre();
    mg.bSphereRadius2 = ((mesh->MaxVec.x - mg.bSphereCentre.x) + (mesh->MaxVec.y - mg.bSphereCentre.y) + (mesh->MaxVec.z - mg.bSphereCentre.z)) / 3.0f;
    mg.bSphereRadius2 *= mg.bSphereRadius2;

    UpdateTempMat(mg);

    mg.meshDirty = false;
}

bool STLRenderer::TestMeshIntersection(Mesh *mesh, float screenX, float screenY, float &screenZ)
{
    // Calculate far and near points
    glm::vec4 origin = glm::vec4(screenX, screenY, 0, 1);
    glm::vec4 far = glm::vec4(screenX, screenY, 1, 1);

    // TODO: cache these matricies
    glm::mat4 VP = meshGroups[mesh].sceneMat * ComboRendering::sceneProj;
    glm::mat4 invVP = glm::inverse(VP);
    origin = invVP * origin;
    far = invVP * far;
    glm::vec3 A = glm::vec3(origin);
    glm::vec3 B = glm::vec3(far);
    glm::vec3 AB = B - A;
    //glm::vec3 rayDir = glm::normalize(AB);

    // Calculate the closest point on the ray from the sphere
    double ab_square = glm::dot(AB, AB);
    glm::vec3 &P = meshGroups[mesh].bSphereCentre;
    glm::vec3 AP = P - A;
    double ap_dot_ab = glm::dot(AP, AB);
    // t is a projection param when we project vector AP onto AB
   float t = (float)(ap_dot_ab / ab_square);
    glm::vec3 Q = A + AB * t;

    // Calculate the collision point if it exists
    if (t >= 0.0 && t <= 1.0)
    {
        // It is inside the clipping planes

        double len = glm::distance2(Q, P);
        if (len < (meshGroups[mesh].bSphereRadius2 + 2.0f))
        {
            // We have a collision, now we need to determine the point
            glm::vec4 point = glm::vec4(Q, 1.0f);
            point = VP * point;
            screenZ = point.z;
            return true;
        }
    }

    return false;
}

void STLRenderer::ColorMesh(Mesh *mesh, glm::vec4 colorAlpha)
{
    meshGroups[mesh].color = colorAlpha;
}

void STLRenderer::ColorMesh(Mesh *mesh, glm::vec3 color)
{
    meshGroups[mesh].color = glm::vec4(color, meshGroups[mesh].color.w);
}

void STLRenderer::ColorMesh(Mesh *mesh, float alpha)
{
    meshGroups[mesh].color.w = alpha;
}

void STLRenderer::ColorAll(glm::vec4 colorAlpha)
{
    for (auto &pair : meshGroups)
        pair.second.color = colorAlpha;
}

void STLRenderer::ColorAll(glm::vec3 color)
{
    for (auto &pair : meshGroups)
        pair.second.color = glm::vec4(color, pair.second.color.w);
}

void STLRenderer::ColorAll(float alpha)
{
    for (auto &pair : meshGroups)
        pair.second.color.w = alpha;
}

void STLRenderer::Init()
{
    // Shader source files
    const std::string vs = "mesh.vsh";
    const std::string fs = "minimal.fsh";

    // Set up the shader and its uniform/attribute locations.
    mProgram = GLHelper::CompileProgramFromFile(vs, fs);
    mPositionAttribLocation = glGetAttribLocation(mProgram, "aPosition");
    mModelUniformLocation = glGetUniformLocation(mProgram, "uModelMatrix");
    mProjUniformLocation = glGetUniformLocation(mProgram, "uProjMatrix");
    mNormalAttribLocation = glGetAttribLocation(mProgram, "aNormal");
    mNormUniformLocation = glGetUniformLocation(mProgram, "uNormMatrix");
    mColorUniformLocation = glGetUniformLocation(mProgram, "uMeshColor");
}

void STLRenderer::Draw()
{
    if (mProgram == 0)
        return;

    glUseProgram(mProgram);

    if (dirtyMesh)
    {
        for (auto &gPair : meshGroups)
        {
            if (gPair.second.meshDirty)
                LoadMesh(gPair.second, gPair.first);
        }

        dirtyMesh = false;
    }

    if (dirtyProjMat)
    {
        glUniformMatrix4fv(mProjUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneProj));
        dirtyProjMat = false;
    }

    for (auto &gPair : meshGroups)
    {
        MeshGroupData &mg = gPair.second;

        if (mg.mVertexPositionBuffer == 0 || mg.mVertexNormalBuffer == 0)
            continue;

        // Set the matrices to those of this mesh, update if needed first
        if (mg.sceneMatsDirty || dirtySceneMat)
        {
            mg.sceneMat = ComboRendering::sceneTrans * mg.gpuMat;
            mg.normalMat = glm::inverse(mg.sceneMat);
            mg.sceneMatsDirty = false;
        }
        glUniformMatrix4fv(mModelUniformLocation, 1, GL_FALSE, glm::value_ptr(mg.sceneMat));
        glUniformMatrix4fv(mNormUniformLocation, 1, GL_FALSE, glm::value_ptr(mg.normalMat));

        // Set the colour for the mesh
        glUniform4fv(mColorUniformLocation, 1, glm::value_ptr(mg.color));

        glBindBuffer(GL_ARRAY_BUFFER, mg.mVertexPositionBuffer);
        glEnableVertexAttribArray(mPositionAttribLocation);
        glVertexAttribPointer(mPositionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, mg.mVertexNormalBuffer);
        glEnableVertexAttribArray(mNormalAttribLocation);
        glVertexAttribPointer(mNormalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLES, 0, gPair.first->trigCount * 3);
    }

    dirtySceneMat = false;
}
