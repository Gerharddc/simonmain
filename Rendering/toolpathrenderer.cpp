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

    if (groupDatas != nullptr)
    {
        delete[] groupDatas;
        groupDatas = nullptr;
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

GroupGLData::~GroupGLData()
{
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

    if (indices != nullptr)
    {
        delete[] indices;
        indices = nullptr;
    }
}

void ToolpathRenderer::LoadPath()
{
    dirtyPath = false;

    if (groupDatas != nullptr)
        delete[] groupDatas;

    auto chunks = path->CalculateDataChunks();
    groupCount = chunks->size();
    groupDatas = new GroupGLData[groupCount];

    for (std::size_t i = 0; i < groupCount; i++)
    {
        TPDataChunk *dc = &chunks->at(i);
        GroupGLData *gd = groupDatas + i;

        glGenBuffers(1, &gd->mCurPosBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, gd->mCurPosBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * dc->curFloatCount, dc->curFloats, GL_STATIC_DRAW);

        glGenBuffers(1, &gd->mNextPosBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, gd->mNextPosBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * dc->nextFloatCount, dc->nextFloats, GL_STATIC_DRAW);

        glGenBuffers(1, &gd->mPrevPosBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, gd->mPrevPosBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * dc->prevFloatCount, dc->prevFloats, GL_STATIC_DRAW);

        glGenBuffers(1, &gd->mSideBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, gd->mSideBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * dc->sideFloatCount, dc->sides, GL_STATIC_DRAW);

        gd->indices = dc->getIndices();
        gd->idxCount = dc->idxCount;
    }

    delete chunks;
}

void ToolpathRenderer::Init()
{
    // Shader source files
    //const std::string vs = "line.vsh";
    const std::string vs = "filament.vsh";
    const std::string fs = "filament.fsh";

    // Set up the shader and its uniform/attribute locations.
    mProgram = GLHelper::CompileProgramFromFile(vs, fs);
    mModelUniformLocation = glGetUniformLocation(mProgram, "uModelMatrix");
    mRadiusUniformLocation = glGetUniformLocation(mProgram, "uFilamentRadius");

    mProjUniformLocation = glGetUniformLocation(mProgram, "uProjMatrix");
    mCurPosAttribLocation = glGetAttribLocation(mProgram, "aCurPos");
    mNextPosAttribLocation = glGetAttribLocation(mProgram, "aNextPos");
    mPrevPosAttribLocation = glGetAttribLocation(mProgram, "aPrevPos");
    mSideAttribLocation = glGetAttribLocation(mProgram, "aSide");
}

void ToolpathRenderer::Draw()
{
    if (mProgram == 0)
        return;

    glUseProgram(mProgram);

    if (dirtyPath)
        LoadPath();

    if (dirtySceneMat)
    {
        glUniformMatrix4fv(mModelUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneTrans));
        dirtySceneMat = false;
    }

    if (dirtyProjMat)
    {
        glUniformMatrix4fv(mProjUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneProj));
        dirtyProjMat = false;
    }

    for (std::size_t i = 0; i < groupCount; i++)
    {
        GroupGLData *ld = groupDatas + i;

        glBindBuffer(GL_ARRAY_BUFFER, ld->mCurPosBuffer);
        glEnableVertexAttribArray(mCurPosAttribLocation);
        glVertexAttribPointer(mCurPosAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, ld->mNextPosBuffer);
        glEnableVertexAttribArray(mNextPosAttribLocation);
        glVertexAttribPointer(mNextPosAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, ld->mPrevPosBuffer);
        glEnableVertexAttribArray(mPrevPosAttribLocation);
        glVertexAttribPointer(mPrevPosAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, ld->mSideBuffer);
        glEnableVertexAttribArray(mSideAttribLocation);
        glVertexAttribPointer(mSideAttribLocation, 1, GL_FLOAT, GL_FALSE, 0, 0);

        glUniform1f(mRadiusUniformLocation, 0.25f);

        glDrawElements(GL_TRIANGLES, ld->idxCount, GL_UNSIGNED_SHORT, ld->indices);
    }
}
