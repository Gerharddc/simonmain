#ifndef STRUCTURES
#define STRUCTURES

#include <stdexcept>
#include <Misc/strings.h>
#include <cstring>
#include <limits>

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

class Mesh
{
private:
    //Triangle *trigs;
    int trigCount;

public:
    Vec3 MinVec;
    Vec3 MaxVec;

    float *vertexFloats;
    short *indices;
    float *normalFloats;

    std::size_t vertexCount = 0;

    Mesh(int size)
    {
        //trigs = new Triangle[size];
        trigCount = size;
        vertexFloats = new float[size * 3 * 3]; //TODO: maybe smaller
        indices = new short[size * 3];
        normalFloats = new float[size * 3];
    }

    ~Mesh()
    {
        //delete[] trigs;
        delete[] vertexFloats;
        delete[] indices;
        delete[] normalFloats;
    }

    int TrigCount() { return trigCount; }

    void Resize(std::size_t newSize)
    {
        // Recreate the arrays with the new size
        trigCount = newSize;
        float *vF = new float[vertexCount * 3];
        short *i = new short[newSize * 3];
        float *nF = new float[newSize * 3];

        memcpy(vF, vertexFloats, sizeof(float) * vertexCount * 3);
        memcpy(i, indices, sizeof(short) * newSize * 3);
        memcpy(nF, normalFloats, sizeof(float) * newSize * 3);

        delete vertexFloats;
        delete indices;
        delete normalFloats;

        vertexFloats = vF;
        indices = i;
        normalFloats = nF;
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

