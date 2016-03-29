#ifndef STLRENDERER_H
#define STLRENDERER_H

#ifdef GLES
#include <GLES2/gl2.h>
#elif defined(QT_APPLICATION)
#include "loadedgl.h"
#endif

#include <map>
#include <thread>
#include <glm/gtx/quaternion.hpp>
#include "structures.h"

struct MeshGroupData
{
    bool meshDirty = false;

    GLuint mVertexPositionBuffer = 0;
    GLuint mVertexNormalBuffer = 0;

    // The scale that the actual mesh is currently on
    float scaleOnMesh = 1.0f;
    // The scale that the matrix is applying to the mesh
    float scaleOnMat = 1.0f;

    // The matrix that is currently being GPU-applied to the mesh
    glm::mat4 gpuMat;
    // The matrix that has already been applied to the mesh as seen by the cpu
    glm::mat4 meshMat;

    // Thes matrices and flag are used to store the combined mesh and scene matrices
    glm::mat4 sceneMat, normalMat;
    bool sceneMatsDirty = true;

    // The colour of the mesh with transparency
    glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    // The rotation that the actual mesh currently has
    glm::vec3 rotOnMesh;
    // The rotation that the matrix is applying to the mesh
    glm::vec3 rotOnMat;

    // This is the negative coordinate at the centre of the mesh
    glm::vec3 meshCentre;
    // The offset that the actual mesh currently has
    glm::vec3 moveOnMesh;
    // The offset that the matrix is applying to the mesh
    glm::vec3 moveOnMat;

    // The centre of the bounding sphere
    glm::vec3 bSphereCentre;
    // The squared (performance) radius of the bounding sphere
    double bSphereRadius2;

    void Destroy();
};

class STLRenderer
{
public:
    STLRenderer();
    ~STLRenderer();
    void Draw();
    void Init();

    void AddMesh(Mesh *mesh);
    void RemoveMesh(Mesh *mesh);

    void ScaleMesh(Mesh *mesh, float absScale);
    void CentreMesh(Mesh *mesh, float absX, float absY);
    void LiftMesh(Mesh *mesh, float absZ);
    void RotateMesh(Mesh *mesh, float absX, float absY, float absZ);

    void ColorMesh(Mesh *mesh, glm::vec4 colorAlpha);
    void ColorMesh(Mesh *mesh, glm::vec3 color);
    void ColorMesh(Mesh *mesh, float alpha);

    void ColorAll(glm::vec4 colorAlpha);
    void ColorAll(glm::vec3 color);
    void ColorAll(float alpha);

    bool TestMeshIntersection(Mesh *mesh, float screenX, float screenY, float &screenZ);

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
    GLint mColorUniformLocation;

    void LoadMesh(MeshGroupData &mg, Mesh *mesh);
    std::map<Mesh*, MeshGroupData> meshGroups;

    // We need flags to determine when matrices have changed as
    // to be able to give new ones to opengl
    bool dirtyProjMat = true;
    bool dirtySceneMat = true;
    bool dirtyMesh = false;
};

#endif // STLRENDERER_H
