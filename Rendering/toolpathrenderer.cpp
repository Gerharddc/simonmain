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

    if (layerDatas != nullptr)
    {
        delete[] layerDatas;
        layerDatas = nullptr;
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

LayerGLData::~LayerGLData()
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

    if (layerDatas != nullptr)
        delete[] layerDatas;

    layerCount = path->layers.size();

    layerDatas = new LayerGLData[layerCount];

    for (std::size_t i = 0; i < layerCount; i++)
    {
        LayerData *cld = path->CalculateLayerData(i);
        LayerGLData *ld = layerDatas + i;

        glGenBuffers(1, &ld->mCurPosBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, ld->mCurPosBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cld->pointFloatCount, cld->curFloats, GL_STATIC_DRAW);

        glGenBuffers(1, &ld->mNextPosBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, ld->mNextPosBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cld->pointFloatCount, cld->nextFloats, GL_STATIC_DRAW);

        glGenBuffers(1, &ld->mPrevPosBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, ld->mPrevPosBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cld->pointFloatCount, cld->prevFloats, GL_STATIC_DRAW);

        glGenBuffers(1, &ld->mSideBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, ld->mSideBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cld->sideFloatCount, cld->sides, GL_STATIC_DRAW);

        ld->indices = cld->getIndices();
        ld->layerHeight = path->layers[i].z;
        ld->idxCount = cld->idxCount;

        delete cld;
    }
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
    mZUniformLocation = glGetUniformLocation(mProgram, "uLayerZ");

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
        glUniformMatrix4fv(mModelUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneTrans));

    if (dirtyProjMat)
        glUniformMatrix4fv(mProjUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneProj));

    for (std::size_t i = 0; i < layerCount; i++)
    {
        LayerGLData *ld = layerDatas + i;

        glBindBuffer(GL_ARRAY_BUFFER, ld->mCurPosBuffer);
        glEnableVertexAttribArray(mCurPosAttribLocation);
        glVertexAttribPointer(mCurPosAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, ld->mNextPosBuffer);
        glEnableVertexAttribArray(mNextPosAttribLocation);
        glVertexAttribPointer(mNextPosAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, ld->mPrevPosBuffer);
        glEnableVertexAttribArray(mPrevPosAttribLocation);
        glVertexAttribPointer(mPrevPosAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, ld->mSideBuffer);
        glEnableVertexAttribArray(mSideAttribLocation);
        glVertexAttribPointer(mSideAttribLocation, 1, GL_FLOAT, GL_FALSE, 0, 0);

        glUniform1f(mRadiusUniformLocation, 0.5f);
        glUniform1f(mZUniformLocation, ld->layerHeight);

        glDrawElements(GL_TRIANGLES, ld->idxCount, GL_UNSIGNED_SHORT, ld->indices);
    }
}
