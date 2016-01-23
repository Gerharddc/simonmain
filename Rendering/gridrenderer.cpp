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
    mViewUniformLocation = glGetUniformLocation(mProgram, "uViewMatrix");
    mProjUniformLocation = glGetUniformLocation(mProgram, "uProjMatrix");

    glGenBuffers(1, &mVertexPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * grid->floatCount, grid->floats, GL_STATIC_DRAW);
    delete grid;
}

void GridRenderer::Draw()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (mProgram == 0)
        return;

    glUseProgram(mProgram);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
    glEnableVertexAttribArray(mPositionAttribLocation);
    glVertexAttribPointer(mPositionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glm::mat4 trans;
    trans = glm::rotate(trans, glm::radians((float)mDrawCount / 5.0f), glm::vec3(0.5f, 0.5f, 0.0f));
    trans = glm::scale(trans, glm::vec3(1.0f, 1.0f, 1.0f));
    glUniformMatrix4fv(mModelUniformLocation, 1, GL_FALSE, glm::value_ptr(trans));

    glm::mat4 view = glm::lookAt(glm::vec3(50.0f, 50.0f, 150.0f), glm::vec3(50.0f, 50.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(mViewUniformLocation, 1, GL_FALSE, glm::value_ptr(view));

    MathHelper::Matrix4 projectionMatrix = MathHelper::SimpleProjectionMatrix(float(mWindowWidth) / float(mWindowHeight));
    glUniformMatrix4fv(mProjUniformLocation, 1, GL_FALSE, &(projectionMatrix.m[0][0]));

    glDrawArrays(GL_LINES, 0, vertCount);

    mDrawCount += 1;
}

void GridRenderer::UpdateWindowSize(GLsizei width, GLsizei height)
{
    glViewport(0, 0, width, height);
    mWindowWidth = width;
    mWindowHeight = height;
}