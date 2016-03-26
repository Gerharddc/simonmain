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

void STLRenderer::AddMesh(Mesh *_mesh)
{
    // Add a new mesh data group to the vector that contains all the metadata and helpers
    MeshGroupData mg;
    mg.meshDirty = true;
    meshGroups.emplace(_mesh, mg);

    // Signal the global dirty mesh flag
    dirtyMesh = true;
}

void STLRenderer::RemoveMesh(Mesh *_mesh)
{
    // Destroy the mesh and then remove it
    meshGroups[_mesh].Destroy();
    meshGroups.erase(_mesh);
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

    if (dirtySceneMat)
    {
        // Update the model and normal matrices
        glUniformMatrix4fv(mModelUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneTrans));
        glUniformMatrix4fv(mNormUniformLocation, 1, GL_FALSE,
                           glm::value_ptr(glm::inverse(ComboRendering::sceneTrans)));
        dirtySceneMat = false;
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

        glBindBuffer(GL_ARRAY_BUFFER, mg.mVertexPositionBuffer);
        glEnableVertexAttribArray(mPositionAttribLocation);
        glVertexAttribPointer(mPositionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, mg.mVertexNormalBuffer);
        glEnableVertexAttribArray(mNormalAttribLocation);
        glVertexAttribPointer(mNormalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLES, 0, gPair.first->trigCount * 3);
    }
}
