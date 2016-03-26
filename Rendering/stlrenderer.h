#ifndef STLRENDERER_H
#define STLRENDERER_H

#ifdef GLES
#include <GLES2/gl2.h>
#elif defined(QT_APPLICATION)
#include "loadedgl.h"
#endif

#include <map>
#include "structures.h"

struct MeshGroupData
{
    bool meshDirty = false;

    GLuint mVertexPositionBuffer = 0;
    GLuint mVertexNormalBuffer = 0;

    float centreX = 0.0f;
    float centreY = 0.0f;

    void Destroy();
};

class STLRenderer
{
public:
    STLRenderer();
    ~STLRenderer();
    void Draw();
    void Init();
    void AddMesh(Mesh *_mesh);
    void RemoveMesh(Mesh *_mesh);

    void ProjMatDirty();
    void SceneMatDirty();

private:
    GLuint mProgram = 0;
    GLsizei mWindowWidth = 0;
    GLsizei mWindowHeight = 0;

    GLint mPositionAttribLocation;
    GLint mNormalAttribLocation;

    GLint mModelUniformLocation;
    GLint mProjUniformLocation;
    GLint mNormUniformLocation;

    void LoadMesh(MeshGroupData &mg, Mesh *mesh);
    std::map<Mesh*, MeshGroupData> meshGroups;

    // We need flags to determine when matrices have changed as
    // to be able to give new ones to opengl
    bool dirtyProjMat = true;
    bool dirtySceneMat = true;
    bool dirtyMesh = false;
};

#endif // STLRENDERER_H
