#include "toolpathrendering.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glhelper.h"
#include "mathhelper.h"
#include "gcodeimporting.h"
#include "comborendering.h"

namespace ToolpathRendering {
    struct GroupGLData
    {
        ~GroupGLData();

        GLuint mCurPosBuffer = 0;
        GLuint mNextPosBuffer = 0;
        GLuint mPrevPosBuffer = 0;
        GLuint mSideBuffer = 0;

        uint16_t *indices;
        short idxCount = 0;
    };

    GLuint mProgram = 0;
    GLsizei mWindowWidth = 0;
    GLsizei mWindowHeight = 0;

    GLint mCurPosAttribLocation = 0;
    GLint mNextPosAttribLocation = 0;
    GLint mPrevPosAttribLocation = 0;
    GLint mSideAttribLocation = 0;

    GLint mModelUniformLocation = 0;
    GLint mRadiusUniformLocation = 0;
    GLint mProjUniformLocation = 0;
    GLint mColorUniformLocation = 0;

    GroupGLData *groupDatas = nullptr;
    std::size_t groupCount = 0;

    inline void LoadPath();
    Toolpath *path;

    // We need flags to determine when matrices have changed as
    // to be able to give new ones to opengl
    bool dirtyProjMat = true;
    bool dirtySceneMat = true;
    bool dirtyPath = false;
    bool dirtyColor = true;

    glm::vec3 _color = glm::vec3(0.2f, 0.2f, 0.8f);
    float opacity = 1.0f;
}

void ToolpathRendering::FreeMemory()
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

void ToolpathRendering::SceneMatDirty()
{
    dirtySceneMat = true;
}

void ToolpathRendering::ProjMatDirty()
{
    dirtyProjMat = true;
}

void ToolpathRendering::SetToolpath(Toolpath *tp)
{
    path = tp;
    dirtyPath = true;
}

ToolpathRendering::GroupGLData::~GroupGLData()
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

void ToolpathRendering::LoadPath()
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

void ToolpathRendering::Init()
{
    // Shader source files
    //const std::string vs = "line.vsh";
    const std::string vs = "filament.vsh";
    const std::string fs = "filament.fsh";

    // Set up the shader and its uniform/attribute locations.
    mProgram = GLHelper::CompileProgramFromFile(vs, fs);
    mModelUniformLocation = glGetUniformLocation(mProgram, "uModelMatrix");
    mRadiusUniformLocation = glGetUniformLocation(mProgram, "uFilamentRadius");
    mColorUniformLocation = glGetUniformLocation(mProgram, "uColor");

    mProjUniformLocation = glGetUniformLocation(mProgram, "uProjMatrix");
    mCurPosAttribLocation = glGetAttribLocation(mProgram, "aCurPos");
    mNextPosAttribLocation = glGetAttribLocation(mProgram, "aNextPos");
    mPrevPosAttribLocation = glGetAttribLocation(mProgram, "aPrevPos");
    mSideAttribLocation = glGetAttribLocation(mProgram, "aSide");
}

void ToolpathRendering::Draw()
{
    if (mProgram == 0)
        return;

    glUseProgram(mProgram);

    if (dirtyPath)
        LoadPath();

    if (dirtySceneMat)
    {
        glUniformMatrix4fv(mModelUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::getSceneTrans()));
        dirtySceneMat = false;
    }

    if (dirtyProjMat)
    {
        glUniformMatrix4fv(mProjUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::getSceneProj()));
        dirtyProjMat = false;
    }

    if (dirtyColor)
    {
        glUniform4fv(mColorUniformLocation, 1, glm::value_ptr(glm::vec4(_color, opacity)));
        dirtyColor = false;
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

        glUniform1f(mRadiusUniformLocation, 0.2f);

        glDrawElements(GL_TRIANGLES, ld->idxCount, GL_UNSIGNED_SHORT, ld->indices);
    }
}

void ToolpathRendering::SetOpacity(float alpha)
{
    opacity = alpha;
    dirtyColor = true;

    // Call OpenGL upate
    ComboRendering::Update();
}

float ToolpathRendering::GetOpacity()
{
    return opacity;
}

void ToolpathRendering::SetColor(glm::vec3 color)
{
    _color = color;
    dirtyColor = true;
}
