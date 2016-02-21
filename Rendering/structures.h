#ifndef STRUCTURES
#define STRUCTURES

#include <stdexcept>
#include <Misc/strings.h>
#include <cstring>
#include <limits>
#include <vector>
#include <glm/glm.hpp>

// The unit that defines the component of vectors
typedef float veccomp;

class Vector3
{
public:
    veccomp x = 0;
    veccomp y = 0;
    veccomp z = 0;

    void ToMax()
    {
        x = y = z = std::numeric_limits<veccomp>::max();
    }

    void ToMin()
    {
        x = y = z = std::numeric_limits<veccomp>::lowest();
    }
};

typedef Vector3 Vec3;

struct Triangle;

struct Vertex
{
    std::vector<std::size_t> trigIdxs;
    //std::size_t trigIdxs[10];
};

struct Triangle
{
    std::size_t vertIdxs[3];
};

// This is not a safe class and should be used carefully because it has almost
// no defences against logical mistakes. This because it should only be used in high
// performance and well tested areas of the program.
class Mesh
{
private:
    float *vertFloats = nullptr;
    float *normFloats = nullptr;

    std::size_t vertArrCount = 0;

public:
    Vec3 MinVec;
    Vec3 MaxVec;

    float *vertexFloats;
    std::size_t *indices;
    Vertex *vertices;
    Triangle *trigs;
    std::size_t trigCount;

    std::size_t vertexCount = 0;

    Mesh(std::size_t size);
    ~Mesh();

    void ShrinkVertices(std::size_t vertCount);
    void ShrinkTrigs(std::size_t newSize);

    Vec3 Centre()
    {
        Vec3 centre;
        centre.x = (MinVec.x + MaxVec.x) / 2;
        centre.y = (MinVec.y + MaxVec.y) / 2;
        centre.z = (MinVec.z + MaxVec.z) / 2;
        return centre;
    }


    // These functions temporarily create an array of repetead vertices for flat shading
    // it is recommended to delete the values directly after passing them to the GPU
    float *getFlatVerts();
    void dumpFlatVerts()
    {
        delete[] vertFloats;
        vertFloats = nullptr;
    }

    // These functions temporarily create an array of repetead vertices for flat shading
    // it is recommended to delete the values directly after passing them to the GPU
    float *getFlatNorms();
    void dumpFlatNorms()
    {
        delete[] normFloats;
        normFloats = nullptr;
    }
};

enum GType
{
    RapidMove = 0,
    Move = 1,
    Home = 28,
    ToInch = 20,
    ToMM = 21,
    SetAbs = 90,
    SetRel =  91,
    SetPos = 92
};

struct Point2
{
    float x = 0;
    float y = 0;

    Point2() {}
    Point2(float _x, float _y) : x(_x), y(_y) {}
};

struct Point3
{
    float x = 0;
    float y = 0;
    float z = 0;

    Point3() {}
    Point3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

struct Island
{
    std::vector<Point3> movePoints;
    std::vector<Point2> printPoints;
};

struct Layer
{
    float z;
    std::vector<Island> islands;
};

class Toolpath;

class LayerData
{
    friend class Toolpath;

private:
    short *indices;

public:
    float *curFloats;
    float *prevFloats;
    float *nextFloats;
    float *sides;
    float *travelFloats;

    short isleCount = 0;
    short pointFloatCount = 0;
    short sideFloatCount = 0;
    short idxCount = 0;
    short travelCount = 0;

    bool indicesCopied = false;

    short *getIndices()
    {
        indicesCopied = true;
        return indices;
    }

    ~LayerData()
    {
        delete[] curFloats;
        delete[] prevFloats;
        delete[] nextFloats;
        delete[] sides;
        delete[] travelFloats;

        // The indices won't be copied over to the gpu and will need to remain in existence
        // the renderer will be responsible for freeing the memory used by that array
        if (!indicesCopied)
        {
            delete[] indices;
            indices = nullptr;
        }
    }
};

struct Toolpath
{
    std::vector<Layer> layers;

    LayerData *CalculateLayerData(std::size_t layerNum);
};

#endif // STRUCTURES

