#include "gridrenderer.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glhelper.h"
#include "mathhelper.h"
#include "comborendering.h"

GridRenderer::GridRenderer(uint xSize, uint ySize, uint zSize, uint interval)
{
    grid = GridGeneration::GenerateGrids(xSize, ySize, zSize, interval);
    vertCount = grid->floatCount / 3;
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
    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * grid->floatCount, grid->floats, GL_STATIC_DRAW);
    delete grid;
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
        dirtySceneMat = false;
        glUniformMatrix4fv(mModelUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneTrans));
    }

    // Apply the new projection matrix if it has changed
    if (dirtyProjMat)
    {
        dirtyProjMat = false;
        glUniformMatrix4fv(mProjUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneProj));
    }

    glDrawArrays(GL_LINES, 0, vertCount);
}
