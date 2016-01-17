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

    Mesh(std::size_t size)
    {
        trigCount = size;

        // Malloc is used to simply the shrinking of the arrays later on if needed
        vertexFloats = (float*)malloc(sizeof(float) * size * 9); // This will need to be shrunk
        indices = (std::size_t*)malloc(sizeof(std::size_t) * size * 3);
        trigs = (Triangle*)malloc(sizeof(Triangle) * size);

        // The arrays in triangles need to be initialized
        for (std::size_t i = 0; i < size; i++)
            trigs[i] = Triangle();

        // The std::vectors in the vertices need to be initialized, we also need to keep track of how many there are as to be able to
        // properly destruct them later on
        vertices = (Vertex*)malloc(sizeof(Vertex) * size * 3);  // This will need to be shrunk
        for (std::size_t i = 0; i < size * 3; i++)
            vertices[i] = Vertex();
        vertArrCount = size * 3;
    }

    ~Mesh()
    {
        free(vertexFloats);
        free(indices);
        free(trigs);
        delete[] vertFloats;
        delete[] normFloats;

        // Because they contains std::vectors, the vertices have to be destructed first
        for (std::size_t i = 0; i < vertArrCount; i++)
            vertices[i].~Vertex();
        free(vertices);
    }

    void ShrinkVertices(std::size_t vertCount)
    {
        // Recreate the arrays that are linked insize to the amount of vertices
        vertexCount = vertCount;

        // We can simply reallocate the memory for the vertex floats
        vertexFloats = (float*)realloc(vertexFloats, sizeof(float) * vertexCount * 3);

        // We need to properly dispose of all the vertices (std::vectors) that will be lost
        // and then reallocate the smaller amount of memory
        // WARNING: this has never failed but the complex nature of std::vector does provide a chance of it
        // perhaps causing strange behaviour when copied directly, please keep this in mind when debugging...
        for (std::size_t i = vertexCount; i < vertArrCount; i++)
        {
            vertices[i].~Vertex();
        }
        vertices = (Vertex*)realloc(vertices, sizeof(Vertex) * vertexCount);
        vertArrCount = vertexCount;
    }

    void ShrinkTrigs(std::size_t newSize)
    {
        // Reallocate the arrays with the new size
        trigCount = newSize;
        indices = (std::size_t*)realloc(indices, sizeof(std::size_t) * newSize * 3);
        trigs = (Triangle*)realloc(trigs, sizeof(Triangle) * newSize);
    }

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
    float *getFlatVerts()
    {
        if (vertexFloats != nullptr)
            delete[] vertFloats;

        vertFloats = new float[trigCount * 9];
        for (std::size_t i = 0; i < trigCount; i++)
        {
            for (uint8_t j = 0; j < 3; j++)
            {
                auto idx = trigs[i].vertIdxs[j];

                for (uint8_t k = 0; k < 3; k++)
                {
                    vertFloats[(i * 9) + (j * 3) + k] = vertexFloats[(idx * 3) + k];
                }
            }
        }
        return vertFloats;
    }
    void dumpFlatVerts()
    {
        delete[] vertFloats;
        vertFloats = nullptr;
    }

    // These functions temporarily create an array of repetead vertices for flat shading
    // it is recommended to delete the values directly after passing them to the GPU
    float *getFlatNorms()
    {
        if (normFloats != nullptr)
            delete[] vertexFloats;

        normFloats = new float[trigCount * 9];
        for (std::size_t i = 0; i <trigCount; i++)
        {
            glm::vec3 vecs[3];

            for (uint8_t j = 0; j < 3; j++)
            {
                auto idx = (indices[i * 3 + j]) * 3;
                vecs[j].x = vertexFloats[idx];
                vecs[j].y = vertexFloats[idx + 1];
                vecs[j].z = vertexFloats[idx + 2];
            }

            glm::vec3 e1 = vecs[1] - vecs[0];
            glm::vec3 e2 = vecs[2] - vecs[0];
            glm::vec3 norm = glm::normalize(glm::cross(e1, e2));

            for (uint8_t j = 0; j < 3; j++)
            {
                normFloats[(i * 9) + (j * 3)] = norm.x;
                normFloats[(i * 9) + (j * 3) + 1] = norm.y;
                normFloats[(i * 9) + (j * 3) + 2] = norm.z;
            }
        }
        return normFloats;
    }
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

struct Layer
{
    float z;
    std::vector<Point> points;
};

class Toolpath
{
public:
    std::vector<Layer*> layers;

    ~Toolpath()
    {
        for (Layer* layer : layers)
            delete layer;
    }
};

#endif // STRUCTURES

