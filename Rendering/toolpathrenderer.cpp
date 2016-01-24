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

    if (mVertexPositionBuffer != 0)
    {
        glDeleteBuffers(1, &mVertexPositionBuffer);
        mVertexPositionBuffer = 0;
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
    if (mVertexPositionBuffer != 0)
        glDeleteBuffers(1, &mVertexPositionBuffer);

    glGenBuffers(1, &mVertexPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * path->getLineCount() * 2 * 3, path->getLineVerts(), GL_STATIC_DRAW);
    path->dumpLineVerts();
}

void ToolpathRenderer::Init()
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
}

void ToolpathRenderer::Draw()
{
    if (mProgram == 0)
        return;

    glUseProgram(mProgram);

    if (dirtyPath)
        LoadPath();

    if (mVertexPositionBuffer == 0)
        return;

    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
    glEnableVertexAttribArray(mPositionAttribLocation);
    glVertexAttribPointer(mPositionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (dirtySceneMat)
        glUniformMatrix4fv(mModelUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneTrans));

    if (dirtyProjMat)
        glUniformMatrix4fv(mProjUniformLocation, 1, GL_FALSE, glm::value_ptr(ComboRendering::sceneProj));

    glDrawArrays(GL_LINES, 0, path->getLineCount() * 2);
}
