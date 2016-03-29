#include "gridrenderer.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glhelper.h"
#include "mathhelper.h"
#include "comborendering.h"
#include "Misc/globalsettings.h"

GridRenderer::GridRenderer()
{

}

GridRenderer::~GridRenderer()
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

    if (grid != nullptr)
        delete grid;
}

void GridRenderer::Init()
{
    // Shader source files
    const std::string vs = "line.vsh";
    const std::string fs = "minimal.fsh";

    // Set up the shader and its uniform/attribute locations.
    mProgram = GLHelper::CompileProgramFromFile(vs, fs);
    mPositionAttribLocation = glGetAttribLocation(mProgram, "aPosition");
    mModelUniformLocation = glGetUniformLocation(mProgram, "uModelMatrix");
    mProjUniformLocation = glGetUniformLocation(mProgram, "uProjMatrix");

    glGenBuffers(1, &mVertexPositionBuffer);
}

void GridRenderer::ProjMatDirty()
{
    dirtyProjMat = true;
}

void GridRenderer::SceneMatDirty()
{
    dirtySceneMat = true;
}

void GridRenderer::GridDirty()
{
    if (grid != nullptr)
        delete grid;


    grid = GridGeneration::GenerateGrids(GlobalSettings::BedWidth.Get(), GlobalSettings::BedLength.Get(), GlobalSettings::BedHeight.Get(), 10.0f);
    vertCount = grid->floatCount / 3;

    dirtyGrid = true;
}

void GridRenderer::Draw()
{
    if (mProgram == 0)
        return;

    glUseProgram(mProgram);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
    glEnableVertexAttribArray(mPositionAttribLocation);
    glVertexAttribPointer(mPositionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Apply the scene tranformation matrix if it has changed
    if (dirtySceneMat)
    {        
        glUniformMatrix4fv(mModelUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneTrans));
        dirtySceneMat = false;
    }

    // Apply the new projection matrix if it has changed
    if (dirtyProjMat)
    {  
        glUniformMatrix4fv(mProjUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneProj));
        dirtyProjMat = false;
    }

    if (vertCount == 0)
        return;

    if (dirtyGrid)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * grid->floatCount, grid->floats, GL_STATIC_DRAW);

        delete grid;
        grid = nullptr;
        dirtyGrid = false;
    }

    glDrawArrays(GL_LINES, 0, vertCount);
}
