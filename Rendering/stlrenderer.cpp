#include "stlrenderer.h"

#include <string>

#include "glhelper.h"
#include "mathhelper.h"
#include "stlimporting.h"

STLRenderer::STLRenderer(Mesh* _mesh)
{
    mesh = _mesh;
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

    if (mVertexColorBuffer != 0)
    {
        glDeleteBuffers(1, &mVertexColorBuffer);
        mVertexColorBuffer = 0;
    }

    if (mIndexBuffer != 0)
    {
        glDeleteBuffers(1, &mIndexBuffer);
        mIndexBuffer = 0;
    }
}

void STLRenderer::Init()
{
    // Shader source files
    const std::string vs = "GL/cube.vsh";
    const std::string fs = "GL/cube.fsh";

    // Set up the shader and its uniform/attribute locations.
    mProgram = GLHelper::CompileProgramFromFile(vs, fs);
    mPositionAttribLocation = glGetAttribLocation(mProgram, "aPosition");
    mColorAttribLocation = glGetAttribLocation(mProgram, "aColor");
    mModelUniformLocation = glGetUniformLocation(mProgram, "uModelMatrix");
    mViewUniformLocation = glGetUniformLocation(mProgram, "uViewMatrix");
    mProjUniformLocation = glGetUniformLocation(mProgram, "uProjMatrix");
    mColorUniformLocation = glGetUniformLocation(mProgram, "uColor");

    glGenBuffers(1, &mVertexPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->vertexCount * 3, mesh->vertexFloats, GL_STATIC_DRAW);

    glGenBuffers(1, &mIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * mesh->TrigCount() * 3, mesh->indices, GL_STATIC_DRAW);
}

void STLRenderer::Draw()
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

    MathHelper::Matrix4 modelMatrix = MathHelper::SimpleModelMatrix((float)mDrawCount / 50.0f);
    glUniformMatrix4fv(mModelUniformLocation, 1, GL_FALSE, &(modelMatrix.m[0][0]));

    MathHelper::Matrix4 viewMatrix = MathHelper::SimpleViewMatrix();
    glUniformMatrix4fv(mViewUniformLocation, 1, GL_FALSE, &(viewMatrix.m[0][0]));

    MathHelper::Matrix4 projectionMatrix = MathHelper::SimpleProjectionMatrix(float(mWindowWidth) / float(mWindowHeight));
    glUniformMatrix4fv(mProjUniformLocation, 1, GL_FALSE, &(projectionMatrix.m[0][0]));

    // All the triangles
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glDrawElements(GL_TRIANGLES, mesh->TrigCount() * 3, GL_UNSIGNED_SHORT, 0);

    mDrawCount += 1;
}

void STLRenderer::UpdateWindowSize(GLsizei width, GLsizei height)
{
    glViewport(0, 0, width, height);
    mWindowWidth = width;
    mWindowHeight = height;
}

