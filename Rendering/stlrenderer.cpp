#include "stlrenderer.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glhelper.h"
#include "mathhelper.h"
#include "stlimporting.h"
#include "comborendering.h"

STLRenderer::STLRenderer()
{

}

// We use pointers to the flags because the MeshGroupData object can potentially
// die before the end of the thread
void SyncMesh(Mesh *mesh, MeshGroupData *mg, bool *syncFlag, bool *delayFlag)
{
    // Terminate the thread if indicated
    while (*syncFlag)
    {
        if (*delayFlag)
        {
            std::this_thread::sleep_for (std::chrono::seconds(15));
            *delayFlag = false;
        }
        else
        {
            *delayFlag = true;

            //Apply the matrix to the mesh
            for (std::size_t i = 0; i < mesh->vertexCount; i++)
            {
                // TODO: optomize...
                auto idx = i * 3;
                glm::vec4 v = glm::vec4(mesh->vertexFloats[idx + 0], mesh->vertexFloats[idx + 1], mesh->vertexFloats[idx + 2], 1.0f);
                v = mg->tempMat * v;
                mesh->vertexFloats[idx + 0] = v.x;
                mesh->vertexFloats[idx + 1] = v.y;
                mesh->vertexFloats[idx + 2] = v.z;
            }

            // Update the parameters

            mg->rotOnMesh *= mg->rotOnMat;
            mg->rotOnMat = glm::vec3(0.0f);

            mg->scaleOnMesh *= mg->scaleOnMat;
            mg->scaleOnMat = 1.0f;

            mg->moveOnMesh += mg->moveOnMat;
            mg->moveOnMat = glm::vec3(0.0f);

            // TODO: maybe just do this from the start
            mg->meshCentre = glm::vec3(0.0f);
        }
    }

    delete syncFlag;
    delete delayFlag;
}

void MeshGroupData::StartThread(Mesh *mesh)
{
    syncFlag = new bool(true);
    delayFlag = new bool(true);

    syncThread = new std::thread(SyncMesh, mesh, this, syncFlag, delayFlag);
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

    // Kill the syncing thread
    *syncFlag = false;
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

// Delay syncing with the mesh
inline void DelaySync(MeshGroupData &mg)
{
    bool *df = mg.delayFlag;

    if (df != nullptr)
        *df = true;
}

// TODO: these transofrmations hould probably be applied in a bg thread

// Update the meshdatagroup matrix
inline void UpdateTempMat(MeshGroupData &mg)
{
    // TODO: optomize ?
    mg.tempMat = glm::translate(glm::mat4(1.0f), mg.moveOnMat);
    mg.tempMat = glm::scale(mg.tempMat, glm::vec3(mg.scaleOnMat));
    mg.tempMat = mg.tempMat * glm::mat4(glm::quat(mg.rotOnMat));
    mg.tempMat = glm::translate(mg.tempMat, mg.meshCentre);

    mg.sceneMatsDirty = true;
}

// This method applies an absolute scale to the original mesh
void STLRenderer::ScaleMesh(Mesh *mesh, float absScale)
{
    MeshGroupData &mg = meshGroups[mesh];
    DelaySync(mg);

    mg.scaleOnMat = absScale - mg.scaleOnMesh;

    UpdateTempMat(mg);
}

// This method centres the mesh around the current coordinates
void STLRenderer::CentreMesh(Mesh *mesh, float absX, float absY)
{
    MeshGroupData &mg = meshGroups[mesh];
    DelaySync(mg);

    mg.moveOnMat.x = absX - mg.moveOnMesh.x;
    mg.moveOnMat.y = absY - mg.moveOnMesh.y;

    UpdateTempMat(mg);
}

// This method places the mesh an absolute height above the bed
void STLRenderer::LiftMesh(Mesh *mesh, float absZ)
{
    MeshGroupData &mg = meshGroups[mesh];
    DelaySync(mg);

    mg.moveOnMat.z = absZ - mg.moveOnMesh.z;

    UpdateTempMat(mg);
}

// This method applies an absolute rotation to the original mesh
void STLRenderer::RotateMesh(Mesh *mesh, float absX, float absY, float absZ)
{
    MeshGroupData &mg = meshGroups[mesh];
    DelaySync(mg);

    mg.rotOnMat = glm::vec3(absX, absY, absZ) - mg.rotOnMesh;

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

    // TODO: move the mesh somewhere
    mg.centreX = (mesh->MinVec.x + mesh->MaxVec.x) / 2;
    mg.centreY = (mesh->MinVec.y + mesh->MaxVec.y) / 2;

    mg.meshDirty = false;
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
            mg.sceneMat = ComboRendering::sceneTrans * mg.tempMat;
            mg.normalMat = glm::inverse(mg.sceneMat);
            mg.sceneMatsDirty = false;
            dirtySceneMat = false;
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
}
