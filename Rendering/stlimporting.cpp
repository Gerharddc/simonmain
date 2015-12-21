#include "stlimporting.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <ios>

#include "Misc/strings.h"

// NOTE: float has to be 32bit real number
// TODO: add check

// The plan here to to allocate as few times as possible and to reuse allocated memory space as much as possilbe
// it is therefor needed to make excessive use of pointers and references.
// This needs to be thought through very well because this code has a large workload and needs to be of as high performance
// as possible whilst using as little memory as possible.

float ReadFloat(std::ifstream &is, char* buf)
{
    is.read(buf, 4);
    return *((float*) buf);
}

void ReadVec3(std::ifstream &is, char* buf, Vec3 &vec)
{
    vec.x = ReadFloat(is, buf);
    vec.y = ReadFloat(is, buf);
    vec.z = ReadFloat(is, buf);
}

inline Mesh* ImportBinary(std::ifstream &is, int trigCount)
{
    Mesh *mesh = new Mesh(trigCount);

    // We allocate this buffer once and resuse it for all the floats
    char buf[4];

    for (int i = 0; i < trigCount; i++)
    {
        ReadVec3(is, buf, (*mesh)[i].Normal);
        ReadVec3(is, buf, (*mesh)[i].A);
        ReadVec3(is, buf, (*mesh)[i].B);
        ReadVec3(is, buf, (*mesh)[i].C);

        // Skip past the empty two bytes at the end
        is.seekg(is.tellg() + 2);
    }

    is.close();
}

inline Mesh* ImportASCII(const char* path)
{
    //std::ifstream is(path, std::ifstream::te)


    is.close();
}

Mesh* ImportSTL(const char *path)
{
    std::ifstream is(path, std::ifstream::binary);
    if (is)
    {
        // get length of file:
        is.seekg (0, is.end);
        int length = is.tellg();
        //is.seekg (0, is.beg);

        // move past the header
        is.seekg(80);

        // read the triangle count
        char trigCountChars[4];
        is.read(trigCountChars, 4);
        ulong trigCount = *((ulong*) trigCountChars);

        // check if the length is correct for binary
        if (length == (84 + (trigCount * 50)))
            return ImportBinary(is, trigCount);
        else
        {
            is.close();
            return ImportASCII(path);
        }
    }
    else
        throw std::runtime_error(format_string("Could not open stl file with path: %s", path));
}
