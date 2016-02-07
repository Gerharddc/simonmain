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

STLRenderer::~STLRenderer()
{
    if (mProgram != 0)
    {
        glDeleteProgram(mProgram);
        mProgram = 0;
    }

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

void STLRenderer::ProjMatDirty()
{
    dirtyProjMat = true;
}

void STLRenderer::SceneMatDirty()
{
    dirtySceneMat = true;
}

int STLRenderer::AddMesh(Mesh *_mesh)
{
    mesher = _mesh;
    dirtyMesh = true;
    return 0; // TODO
}

// TODO: implement multi-mesh support and management
void STLRenderer::LoadMesh(Mesh *mesh)
{
    glGenBuffers(1, &mVertexPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->trigCount * 9, mesh->getFlatVerts(), GL_STATIC_DRAW);
    mesh->dumpFlatVerts();

    glGenBuffers(1, &mVertexNormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexNormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->trigCount * 9, mesh->getFlatNorms(), GL_STATIC_DRAW);
    mesh->dumpFlatNorms();

    x = (mesh->MinVec.x + mesh->MaxVec.x) / 2;
    y = (mesh->MinVec.y + mesh->MaxVec.y) / 2;

    dirtyMesh = false;
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
        LoadMesh(mesher);

    if (mVertexPositionBuffer == 0 || mVertexNormalBuffer == 0)
        return;

    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
    glEnableVertexAttribArray(mPositionAttribLocation);
    glVertexAttribPointer(mPositionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexNormalBuffer);
    glEnableVertexAttribArray(mNormalAttribLocation);
    glVertexAttribPointer(mNormalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (dirtySceneMat)
    {
        // Update the model and normal matrices
        glUniformMatrix4fv(mModelUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneTrans));
        glUniformMatrix4fv(mNormUniformLocation, 1, GL_FALSE,
                           glm::value_ptr(glm::inverse(ComboRendering::sceneTrans)));
    }

    if (dirtyProjMat)
        glUniformMatrix4fv(mProjUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneProj));

    glDrawArrays(GL_TRIANGLES, 0, mesher->trigCount * 3);
}

