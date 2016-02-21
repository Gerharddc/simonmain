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

struct Point
{
    float x = 0;
    float y = 0;
    bool extruded = false;

    bool operator== (Point &b)
    {
        // TODO: this bs is used to skip travel issue
        return (x == b.x && y == b.y);
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

struct Point3
{
    float x,y,z = 0;
};

struct Island
{
    std::vector<Point3> movePoints;
    std::vector<Point> printPoints;
};

struct Layer
{
    float z;
    std::vector<Point> points;

    std::vector<Island> islands;
};

class Toolpath
{
private:
    float *curFloats = nullptr;
    float *prevFloats = nullptr;
    float *nextFloats = nullptr;
    short *indices = nullptr;
    float *sides = nullptr;

    bool indicesCopied = false;

public:
    /*std::vector<Layer*> layers;

    ~Toolpath()
    {
        for (Layer* layer : layers)
            delete layer;
    }*/
    std::vector<Layer> layers;

    void CalculateLayerData(std::size_t layerNum);
    float *getCurFloats();
    float *getNextFloats();
    float *getPrevFloats();
    short *getIndices();
    float *getSides();
    void dumpVertices()
    {
        delete[] curFloats;
        delete[] prevFloats;
        delete[] nextFloats;
        delete[] sides;

        curFloats = nullptr;
        prevFloats = nullptr;
        nextFloats = nullptr;
        sides = nullptr;

        // The indices won't be copied over to the gpu and will need to remain in existence
        // the renderer will be responsible for freeing the memory used by that array
        if (!indicesCopied)
        {
            delete[] indices;
            indices = nullptr;
        }
    }
};

#endif // STRUCTURES

