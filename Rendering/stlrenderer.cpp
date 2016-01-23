#include "stlrenderer.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

    if (mVertexNormalBuffer != 0)
    {
        glDeleteBuffers(1, &mVertexNormalBuffer);
        mVertexNormalBuffer = 0;
    }
}

void STLRenderer::Init()
{
    // Shader source files
    const std::string vs = "mesh.vsh";
    const std::string fs = "minimal.fsh";

    // Set up the shader and its uniform/attribute locations.
    mProgram = GLHelper::CompileProgramFromFile(vs, fs);
    mPositionAttribLocation = glGetAttribLocation(mProgram, "aPosition");
    mModelUniformLocation = glGetUniformLocation(mProgram, "uModelMatrix");
    mProjUniformLocation = glGetUniformLocation(mProgram, "uProjMatrix");
    mNormalAttribLocation = glGetAttribLocation(mProgram, "aNormal");
    mNormUniformLocation = glGetUniformLocation(mProgram, "uNormMatrix");

    glGenBuffers(1, &mVertexPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->trigCount * 9, mesh->getFlatVerts(), GL_STATIC_DRAW);
    mesh->dumpFlatVerts();

    glGenBuffers(1, &mVertexNormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexNormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->trigCount * 9, mesh->getFlatNorms(), GL_STATIC_DRAW);
    mesh->dumpFlatNorms();

    x = (mesh->MinVec.x + mesh->MaxVec.x) / 2;
    y = (mesh->MinVec.y + mesh->MaxVec.y) / 2;
}

void STLRenderer::Draw()
{
    if (mProgram == 0)
        return;

    glUseProgram(mProgram);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
    glEnableVertexAttribArray(mPositionAttribLocation);
    glVertexAttribPointer(mPositionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexNormalBuffer);
    glEnableVertexAttribArray(mNormalAttribLocation);
    glVertexAttribPointer(mNormalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glm::mat4 trans;
    trans = glm::translate(trans, glm::vec3(50.0f, 50.0f, 0.0f));
    trans = glm::rotate(trans, glm::radians((float)mDrawCount / 5.0f), glm::vec3(0.5f, 0.5f, 0.0f));
    trans = glm::translate(trans, glm::vec3(-50.0f, -50.0f, 0.0f));
    trans = glm::translate(trans, glm::vec3(50.0f - x, 50.0f - y, 0.0f));
    glUniformMatrix4fv(mModelUniformLocation, 1, GL_FALSE, glm::value_ptr(trans));

    // Calculate an orthographic projection that centres the view at the aiming position and applies the zoom
    float left = aimX - (centreX / zoom);
    float right = aimX + (centreX / zoom);
    float bottom = aimY - (centreY / zoom);
    float top = aimY + (centreY / zoom);
    // With the orthographic system we need a negative and positive clip plane of enough distance
    glm::mat4 proj = glm::ortho(left, right, bottom, top, -300.0f, 300.0f);
    glUniformMatrix4fv(mProjUniformLocation, 1, GL_FALSE, glm::value_ptr(proj));

    glm::mat4 norm = trans;
    norm = glm::inverse(norm);
    norm = glm::transpose(norm);
    glUniformMatrix4fv(mNormUniformLocation, 1, GL_FALSE, glm::value_ptr(norm));

    glDrawArrays(GL_TRIANGLES, 0, mesh->trigCount * 3);

    mDrawCount += 1;
}

void STLRenderer::UpdateWindowSize(GLsizei width, GLsizei height)
{
    glViewport(0, 0, width, height);
    mWindowWidth = width;
    mWindowHeight = height;

    centreX = width / 2.0f;
    centreY = height / 2.0f;
}

