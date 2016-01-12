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

class Mesh
{
private:
    float *vertFloats = nullptr;
    float *normFloats = nullptr;

public:
    Vec3 MinVec;
    Vec3 MaxVec;

    float *vertexFloats;
    std::size_t *indices;
    Vertex *vertices;
    Triangle *trigs;
    std::size_t trigCount;

    std::size_t vertexCount = 0;

    Mesh(int size)
    {
        trigCount = size;
        vertexFloats = new float[size * 3 * 3]; //TODO: maybe smaller
        indices = new std::size_t[size * 3];
        trigs = new Triangle[size];
        vertices = new Vertex[size * 3]; // TODO: smaller
    }

    ~Mesh()
    {
        delete[] vertexFloats;
        delete[] indices;
        delete[] trigs;
        delete[] vertices;
        delete[] vertFloats;
        delete[] normFloats;
    }

    int TrigCount() { return trigCount; }

    void Resize(std::size_t newSize)
    {
        // Recreate the arrays with the new size
        trigCount = newSize;
        float *vF = new float[vertexCount * 3];
        std::size_t *i = new std::size_t[newSize * 3];
        Triangle *nT = new Triangle[newSize];

        memcpy(vF, vertexFloats, sizeof(float) * vertexCount * 3);
        memcpy(i, indices, sizeof(std::size_t) * newSize * 3);
        memcpy(nT, trigs, sizeof(Triangle) * newSize);

        delete[] vertexFloats;
        delete[] indices;
        delete[] trigs;

        vertexFloats = vF;
        indices = i;
        trigs = nT;
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

#endif // STRUCTURES

