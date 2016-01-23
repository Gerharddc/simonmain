#include "gridrenderer.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glhelper.h"
#include "mathhelper.h"

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
    /*glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/

    if (mProgram == 0)
        return;

    glUseProgram(mProgram);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
    glEnableVertexAttribArray(mPositionAttribLocation);
    glVertexAttribPointer(mPositionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    /*glm::mat4 trans;
    // We need to apply the matrixes in reverse because that is how they work
    trans = glm::translate(trans, glm::vec3(50.0f, 50.0f, 0.0f));
    trans = glm::rotate(trans, glm::radians((float)mDrawCount / 5.0f), glm::vec3(0.5f, 0.5f, 0.0f));
    trans = glm::translate(trans, glm::vec3(-50.0f, -50.0f, 0.0f));
    //trans = glm::scale(trans, glm::vec3(1.0f, 1.0f, 1.0f));
    glUniformMatrix4fv(mModelUniformLocation, 1, GL_FALSE, glm::value_ptr(trans));*/

    // Apply the scene tranformation matrix if it has changed
    if (dirtySceneMat)
    {
        dirtySceneMat = false;
        glUniformMatrix4fv(mModelUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneTrans));
    }

    /*// Calculate an orthographic projection that centres the view at the aiming position and applies the zoom
    float left = aimX - (centreX / zoom);
    float right = aimX + (centreX / zoom);
    float bottom = aimY - (centreY / zoom);
    float top = aimY + (centreY / zoom);
    // With the orthographic system we need a negative and positive clip plane of enough distance
    glm::mat4 proj = glm::ortho(left, right, bottom, top, -300.0f, 300.0f);
    glUniformMatrix4fv(mProjUniformLocation, 1, GL_FALSE, glm::value_ptr(proj));*/

    // Apply the new projection matrix if it has changed
    if (dirtyProjMat)
    {
        dirtyProjMat = false;
        glUniformMatrix4fv(mProjUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneProj));
    }

    glDrawArrays(GL_LINES, 0, vertCount);

    //mDrawCount += 1;
}

/*void GridRenderer::UpdateWindowSize(GLsizei width, GLsizei height)
{
    glViewport(0, 0, width, height);
    mWindowWidth = width;
    mWindowHeight = height;

    centreX = float(width) / 2.0f;
    centreY = float(height) / 2.0f;
}*/
