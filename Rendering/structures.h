#ifndef STRUCTURES
#define STRUCTURES

#include <stdexcept>
#include <Misc/strings.h>
#include <cstring>

// The unit that defines the component of vectors
typedef float veccomp;

struct Vector3
{
    veccomp x;
    veccomp y;
    veccomp z;
};

typedef Vector3 Vec3;

struct Triangle
{
    /*Triangle(Vec3 *norm, Vec3 *a, Vec3 *b, Vec3 *c)
    {
        Normal = norm;
        A = a;
        B = b;
        C = c;
    }

    ~Triangle()
    {
        delete norm;
        delete a;
        delete b;
        delete c;
    }*/

    Vec3 Normal;
    Vec3 A;
    Vec3 B;
    Vec3 C;
};

class Mesh
{
private:
    Triangle *trigs;
    int trigCount;

public:
    Mesh(int size)
    {
        trigs = new Triangle[size];
        trigCount = size;
    }

    ~Mesh()
    {
        delete[] trigs;
    }

    int TrigCount() { return trigCount; }

    Triangle &operator[](int i)
    {
        if (i >= trigCount)
            throw std::runtime_error(format_string("Index %d out of range for mesh.", i));
        else
            return trigs[i];
    }

    void TrigsFromArray(Triangle* arr)
    {
        std::memcpy(trigs, arr, trigCount);
    }
};

#endif // STRUCTURES

