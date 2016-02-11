#include "toolpathrenderer.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glhelper.h"
#include "mathhelper.h"
#include "gcodeimporting.h"
#include "comborendering.h"

ToolpathRenderer::ToolpathRenderer()
{

}

ToolpathRenderer::~ToolpathRenderer()
{
    if (mProgram != 0)
    {
        glDeleteProgram(mProgram);
        mProgram = 0;
    }

    if (mCurPosBuffer != 0)
    {
        glDeleteBuffers(1, &mCurPosBuffer);
        mCurPosBuffer = 0;
    }

    if (mNextPosBuffer != 0)
    {
        glDeleteBuffers(1, &mNextPosBuffer);
        mNextPosBuffer = 0;
    }

    if (mPrevPosBuffer != 0)
    {
        glDeleteBuffers(1, &mPrevPosBuffer);
        mPrevPosBuffer = 0;
    }

    if (mSideBuffer != 0)
    {
        glDeleteBuffers(1, &mSideBuffer);
        mSideBuffer = 0;
    }

    if (mZBuffer != 0)
    {
        glDeleteBuffers(1, &mZBuffer);
        mZBuffer = 0;
    }
}

void ToolpathRenderer::SceneMatDirty()
{
    dirtySceneMat = true;
}

void ToolpathRenderer::ProjMatDirty()
{
    dirtyProjMat = true;
}

void ToolpathRenderer::SetToolpath(Toolpath *tp)
{
    path = tp;
    dirtyPath = true;
}

void ToolpathRenderer::LoadPath()
{
    dirtyPath = false;

    if (mCurPosBuffer != 0)
    {
        glDeleteBuffers(1, &mCurPosBuffer);
        glDeleteBuffers(1, &mNextPosBuffer);
        glDeleteBuffers(1, &mPrevPosBuffer);
        glDeleteBuffers(1, &mSideBuffer);
        glDeleteBuffers(1, &mZBuffer);
    }

    auto lCount = path->getLineCount();

    glGenBuffers(1, &mCurPosBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mCurPosBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (lCount * 12 + 4), path->getCurFloats(), GL_STATIC_DRAW);

    glGenBuffers(1, &mNextPosBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mNextPosBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (lCount * 12 + 4), path->getNextFloats(), GL_STATIC_DRAW);

    glGenBuffers(1, &mPrevPosBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mPrevPosBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (lCount * 12 + 4), path->getPrevFloats(), GL_STATIC_DRAW);

    glGenBuffers(1, &mSideBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mSideBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (lCount * 6 + 2), path->getSides(), GL_STATIC_DRAW);

    glGenBuffers(1, &mZBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mZBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (lCount * 6 + 2), path->getZFloats(), GL_STATIC_DRAW);

    path->dumpVertices();
}

void ToolpathRenderer::Init()
{
    // Shader source files
    //const std::string vs = "line.vsh";
    const std::string vs = "filament.vsh";
    const std::string fs = "minimal.fsh";

    // Set up the shader and its uniform/attribute locations.
    mProgram = GLHelper::CompileProgramFromFile(vs, fs);
    mModelUniformLocation = glGetUniformLocation(mProgram, "uModelMatrix");

    mProjUniformLocation = glGetUniformLocation(mProgram, "uProjMatrix");
    mCurPosAttribLocation = glGetAttribLocation(mProgram, "aCurPos");
    mNextPosAttribLocation = glGetAttribLocation(mProgram, "aNextPos");
    mPrevPosAttribLocation = glGetAttribLocation(mProgram, "aPrevPos");
    mSideAttribLocation = glGetAttribLocation(mProgram, "aSide");
    mZAttribLocation = glGetAttribLocation(mProgram, "aZ");

    //mRadiusUniformLocation = glGetUniformLocation(mProgram, "aFilamentRadius");
}

void ToolpathRenderer::Draw()
{
    if (mProgram == 0)
        return;

    glUseProgram(mProgram);

    if (dirtyPath)
        LoadPath();

    if (mCurPosBuffer == 0)
        return;

    glBindBuffer(GL_ARRAY_BUFFER, mCurPosBuffer);
    glEnableVertexAttribArray(mCurPosAttribLocation);
    glVertexAttribPointer(mCurPosAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, mNextPosBuffer);
    glEnableVertexAttribArray(mNextPosAttribLocation);
    glVertexAttribPointer(mNextPosAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, mPrevPosBuffer);
    glEnableVertexAttribArray(mPrevPosAttribLocation);
    glVertexAttribPointer(mPrevPosAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, mSideBuffer);
    glEnableVertexAttribArray(mSideAttribLocation);
    glVertexAttribPointer(mSideAttribLocation, 1, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, mZBuffer);
    glEnableVertexAttribArray(mZAttribLocation);
    glVertexAttribPointer(mZAttribLocation, 1, GL_FLOAT, GL_FALSE, 0, 0);

    //glUniformFloat

    if (dirtySceneMat)
        glUniformMatrix4fv(mModelUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneTrans));

    if (dirtyProjMat)
        glUniformMatrix4fv(mProjUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneProj));

    //glDrawArrays(GL_LINE_STRIP, 0, path->getLineCount() * 12);
    glDrawElements(GL_TRIANGLES, path->getLineCount() * 12, GL_UNSIGNED_SHORT, path->getIndices());
    //glDrawElements(GL_LINE_STRIP, path->getLineCount() * 12, GL_UNSIGNED_SHORT, path->getIndices());
    //short idxs[] = { 0, 2, 2, 4, 4, 6, 6, 0  ,1, 3, 3, 5, 5, 7, 7, 1 };
    //glDrawElements(GL_LINES, 16, GL_UNSIGNED_SHORT, idxs);
    //short idxs[] = { 2, 4, 6, 8, 10, 12, 14, 0  ,3, 5, 7, 9, 11, 13, 15, 1 };
    //glDrawElements(GL_LINES, 16, GL_UNSIGNED_SHORT, idxs);
    //short idxs[] = { 4, 6, 10, 12, 16, 18, 22, 0 };
    //glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, idxs);
    //short idxs[] = { 14, 16, 15, 15, 16, 17 };
    //short idxs[] = { 15, 17 };
    //glDrawElements(GL_LINES, 2, GL_UNSIGNED_SHORT, idxs);
    // Check indices
}
