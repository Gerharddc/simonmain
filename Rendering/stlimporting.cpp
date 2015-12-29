#include "stlimporting.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <ios>
#include <math.h>
#include <unordered_map>
#include <vector>

#include "Misc/strings.h"

// NOTE: float has to be 32bit real number
// TODO: add check

// The plan here to to allocate as few times as possible and to reuse allocated memory space as much as possilbe
// it is therefor needed to make excessive use of pointers and references.
// This needs to be thought through very well because this code has a large workload and needs to be of as high performance
// as possible whilst using as little memory as possible.

#include <QDebug>

namespace STLImporting
{

float ReadFloat(std::ifstream &is, char* buf)
{
    is.read(buf, 4);
    //return *((float*) buf) / 1.0f;
    float f = *((float*) buf) / 20.0f;
    //qDebug() << f;
    return f;
}

void ReadVec3(std::ifstream &is, char* buf, float *arr)
{
    arr[0] = ReadFloat(is, buf);
    arr[1] = ReadFloat(is, buf);
    arr[2] = ReadFloat(is, buf);
}

/*void ReadVert(std::ifstream &is, char* buf, Vertex &v)
{
    ReadVec3(is, buf, v.x);
}*/

/*inline void SetVecMinMax(Vec3 *minVec, Vec3 *maxVec, Triangle &trig)
{
    minVec.SetMin(trig.A);
    minVec.SetMin(trig.B);
    minVec.SetMin(trig.C);

    maxVec.SetMax(trig.A);
    maxVec.SetMax(trig.B);
    maxVec.SetMax(trig.C);
}*/

struct HashVertex
{
    std::size_t operator()(const float *arrPos) const
    {
        uint32_t a = *((uint32_t*)(&(arrPos[0])));
        uint32_t b = *((uint32_t*)(&(arrPos[1])));
        uint32_t c = *((uint32_t*)(&(arrPos[2])));
        return a*3 + b*5 + c*7;
    }
};

struct CompVertex
{
    bool operator()(const float *arrPos1, const float *arrPos2) const
    {
        return (arrPos1[0] ==  arrPos2[0]) && (arrPos1[1] ==  arrPos2[1]) && (arrPos1[2] ==  arrPos2[2]);
    }
};

inline Mesh* ImportBinary(std::ifstream &is, unsigned int trigCount)
{
    Mesh *mesh = new Mesh(trigCount);
    Vec3 MinVec, MaxVec;
    MinVec.ToMax();
    MaxVec.ToMin();

    // We allocate this buffer once and resuse it for all the floats
    char buf[4];

    // Store a hashtable of the exisiting vertices to elimate duplicates
    std::unordered_map<float*, int, HashVertex, CompVertex> vecTable;

    //uint16_t curIdx = 0;
    //uint16_t saveIdx = 0;

    for (unsigned int i = 0; i < trigCount; i++)
    {
        //qDebug() << "Normal:";
        ReadVec3(is, buf, &(mesh->normalFloats[i * 3]));
        //qDebug() << "V1:";
        ReadVec3(is, buf, &(mesh->vertexFloats[i * 9]));
        //qDebug() << "V2:";
        ReadVec3(is, buf, &(mesh->vertexFloats[i * 9 + 3]));
        //qDebug() << "V3:";
        ReadVec3(is, buf, &(mesh->vertexFloats[i * 9 + 6]));

        for (unsigned short j = i * 3; j < (i * 3 + 3); j++)
            mesh->indices[j] = j;

        /*ReadVec3(is, buf, &(mesh->normalFloats[i * 3])); // Normal

        float* posV[3];
        posV[0] = &(mesh->vertexFloats[curIdx * 3]);
        posV[1] = &(mesh->vertexFloats[curIdx * 3 + 3]);
        posV[2] = &(mesh->vertexFloats[curIdx * 3 + 6]);

        ReadVec3(is, buf, posV[0]);
        ReadVec3(is, buf, posV[1]);
        ReadVec3(is, buf, posV[2]);

        // Determine which vertices are new and get the indices of the old ones
        bool newV[3];
        uint16_t indV[3];
        for (uint8_t j = 0; j < 3; j++)
        {
            newV[j] = vecTable.count(posV[j]) == 0;
            if (!newV[j])
                indV[j] = vecTable[posV[j]];
        }

        for (uint8_t j = 0; j < 3; j++)
        {
            if (newV[j])
            {
                vecTable.emplace(posV[j], curIdx);
                mesh->indices[saveIdx] = curIdx;
                //mesh->indices[i * 3 + j] = curIdx;
                curIdx++;
            }
            else
            {
                mesh->indices[saveIdx] = indV[j];
                //mesh->indices[i * 3 + j] = indV[j];
                for (uint8_t k = j + 1; k < 3; k++)
                {
                    *(posV[k - 1]) = *(posV[k - 1]);
                    posV[k] = posV[k - 1];
                }
            }
            saveIdx++;
        }

        /*ReadVec3(is, buf, &(mesh->vertexFloats[i * 3 * 3]));     // A
        ReadVec3(is, buf, &(mesh->vertexFloats[i * 3 * 3 + 3])); // B
        ReadVec3(is, buf, &(mesh->vertexFloats[i * 3 * 3 + 6])); // C*/

        /*ReadVec3(is, buf, (*mesh)[i].Normal);
        ReadVec3(is, buf, (*mesh)[i].A);
        ReadVec3(is, buf, (*mesh)[i].B);
        ReadVec3(is, buf, (*mesh)[i].C);

        SetVecMinMax(MinVec, MaxVec, (*mesh)[i]);*/

        // Skip past the empty two bytes at the end
        is.seekg((std::size_t)is.tellg() + 2);
    }

    mesh->vertexCount = trigCount * 3;

    is.close();

    //mesh->MinVec = MinVec;
    //mesh->MaxVec = MaxVec;
    //mesh->vertexCount = curIdx;

    return mesh;
}

void ReadVec3(std::string &line, std::size_t &lastPos, Vec3 *vec)
{
    /*veccomp* comps [3] = { &(*vec.x), &(*vec.y), &(*vec.z) };

    // This cuurently assumes the line is properly formatted (unsafe)
    for (uint8_t i = 0; i < 3; i++)
    {
        lastPos = line.find(' ', lastPos);
        std::size_t nextPos = lastPos;

        // We need to have support for multiple spaces next to each other
        bool needNext = true;
        while (needNext)
        {
            nextPos = line.find(' ', nextPos + 1);
            if (nextPos == std::string::npos)
            {
                nextPos = line.length() + 1;
                needNext = false;
            }
            else if (line[nextPos - 1] != ' ' && line[nextPos - 1] != ' ') // Space and tab
                needNext = false;
        }

        std::string sub = line.substr(lastPos + 1, nextPos - lastPos - 1);
        lastPos = nextPos;
        veccomp val = std::stof(sub);
        (*comps[i]) = val;
    }*/
}

// This is a rough (hopefully safe) estimation of the amount of bytes that an ASCII facet
// defination can take up and is used to calculate an overly large buffer
const uint minSize = 100;

inline Mesh* ImportASCII(const char* path, std::size_t fileSize)
{
    /*Mesh* mesh;
    std::ifstream is(path);

    if (is)
    {
        // Allocate a temporary array
        Triangle temp[fileSize / minSize];
        Vec3 MinVec, MaxVec;
        MinVec.ToMax();
        MaxVec.ToMin();

        std::string line;
        int i = 0;
        bool valid = true;

        while (valid && std::getline(is, line))
        {
            if (line.find("facet") != std::string::npos)
            {
                std::size_t normalPos = line.find("normal");
                if (normalPos == std::string::npos)
                    valid = false;
                else
                {
                    ReadVec3(line, normalPos, temp[i].Normal());

                    // Read "outer loop"
                    if (!std::getline(is, line))
                    {
                        valid = false;
                        break;
                    }

                    Vec3* vecs [3] = { &temp[i].A(), &temp[i].B(), &temp[i].C() };
                    for (uint8_t j = 0; j < 3; j++)
                    {
                        if (std::getline(is, line))
                        {
                            std::size_t vertexPos = line.find("vertex");
                            if (vertexPos == std::string::npos)
                            {
                                valid = false;
                                break;
                            }
                            else
                            {
                                ReadVec3(line, vertexPos, *(vecs[j]));
                                MinVec.SetMin(*(vecs[j]));
                                MaxVec.SetMax(*(vecs[j]));
                            }
                        }
                        else
                        {
                            valid = false;
                            break;
                        }
                    }

                    // Read "endloop"
                    if (!std::getline(is, line))
                    {
                        valid = false;
                        break;
                    }

                    // Read "endfacet"
                    if (!std::getline(is, line))
                    {
                        valid = false;
                        break;
                    }

                    i++;
                }
            }
        }

        if (valid)
        {
            mesh = new Mesh(i);
            mesh->TrigsFromArray(temp);
            mesh->MinVec = MinVec;
            mesh->MaxVec = MaxVec;
        }
        else
            throw std::runtime_error("The ASCII file is invalid");
    }
    else
        throw std::runtime_error(format_string("Could not open ASCII stl file with path: %s", path));

    is.close();

    return mesh;*/
}

Mesh* ImportSTL(const char *path)
{
    std::ifstream is(path, std::ifstream::binary);
    if (is)
    {
        // get length of file:
        is.seekg (0, is.end);
        std::size_t length = is.tellg();

        // move past the header
        is.seekg(80);

        // read the triangle count
        char trigCountChars[4];
        is.read(trigCountChars, 4);
        uint trigCount = *((uint*) trigCountChars);

        // check if the length is correct for binary
        if (length == (84 + (trigCount * 50)))
            return ImportBinary(is, trigCount);
        else
        {
            is.close();
            return ImportASCII(path, length);
        }
    }
    else
        throw std::runtime_error(format_string("Could not open stl file with path: %s", path));
}

}
