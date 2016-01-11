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

    // Sets this to the minimum of this and other
    // NOTE: unused
    void SetMin(Vector3& other)
    {
        if (other.x < x)
            x = other.x;
        if (other.y < y)
            y = other.y;
        if (other.z < z)
            z = other.z;
    }

    // NOTE: unused
    void SetMin(Vector3 *other) { SetMin(*other); }

    // Sets this to the minimum of this and other
    // NOTE: unused
    void SetMax(Vector3& other)
    {
        if (other.x > x)
            x = other.x;
        if (other.y > y)
            y = other.y;
        if (other.z > z)
            z = other.z;
    }

    // NOTE: unused
    void SetMax(Vector3 *other) { SetMax(*other); }
};

typedef Vector3 Vec3;

struct Triangle;

struct Vertex
{
    //short idx;
    //std::vector<Triangle*> trigs;
    std::vector<std::size_t> trigIdxs;
};

struct Triangle
{
    //Vertex *vertices[3];
    std::size_t vertIdxs[3];
    //bool cachedNormal = false;
    //glm::vec3 normal;
};

struct Mesh
{
    Vec3 MinVec;
    Vec3 MaxVec;

    float *vertexFloats;
    short *indices;
    float *normalFloats;
    Vertex *vertices;
    Triangle *trigs;
    std::size_t trigCount;

    std::size_t vertexCount = 0;

    Mesh(int size)
    {
        //trigs = new Triangle[size];
        trigCount = size;
        vertexFloats = new float[size * 3 * 3]; //TODO: maybe smaller
        indices = new short[size * 3];
        normalFloats = new float[size * 3 * 3]; // TODO: smaller
        trigs = new Triangle[size];
        vertices = new Vertex[size * 3]; // TODO: smaller
    }

    ~Mesh()
    {
        //delete[] trigs;
        delete[] vertexFloats;
        delete[] indices;
        delete[] normalFloats;
        delete[] trigs;
        delete[] vertices;
    }

    int TrigCount() { return trigCount; }

    void Resize(std::size_t newSize)
    {
        // Recreate the arrays with the new size
        trigCount = newSize;
        float *vF = new float[vertexCount * 3];
        short *i = new short[newSize * 3];
        float *nF = new float[newSize * 3];
        Triangle *nT = new Triangle[newSize];

        memcpy(vF, vertexFloats, sizeof(float) * vertexCount * 3);
        memcpy(i, indices, sizeof(short) * newSize * 3);
        memcpy(nF, normalFloats, sizeof(float) * newSize * 3);
        memcpy(nT, trigs, sizeof(Triangle) * newSize);

        delete[] vertexFloats;
        delete[] indices;
        delete[] normalFloats;
        delete[] trigs;

        vertexFloats = vF;
        indices = i;
        normalFloats = nF;
        trigs = nT;
    }

    /*Triangle &operator[](int i)
    {
        if (i >= trigCount)
            throw std::runtime_error(format_string("Index %d out of range for mesh.", i));
        else
            return trigs[i];
    }*/

    /*void TrigsFromArray(Triangle* arr)
    {
        std::memcpy(trigs, arr, trigCount);
    }*/

    Vec3 Centre()
    {
        Vec3 centre;
        centre.x = (MinVec.x + MaxVec.x) / 2;
        centre.y = (MinVec.y + MaxVec.y) / 2;
        centre.z = (MinVec.z + MaxVec.z) / 2;
        return centre;
    }
};

#endif // STRUCTURES

