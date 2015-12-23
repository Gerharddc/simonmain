#include "stlimporting.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <ios>
#include <math.h>

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
        is.seekg((std::size_t)is.tellg() + 2);
    }

    is.close();

    return mesh;
}

// This is a rough (hopefully safe) estimation of the amount of bytes that an ASCII facet
// defination can take up and is used to calculate an overly large buffer
const uint minSize = 100;

void ReadVec3(std::string &line, std::size_t &lastPos, Vec3 &vec)
{
    veccomp* comps [3] = { &vec.x, &vec.y, &vec.z };

    // This cuurently assumes the line is properly formatted (unsafe)
    for (uint8_t i = 0; i < 3; i++)
    {
        lastPos = line.find(' ', lastPos);
        std::size_t nextPos = line.find(' ', lastPos);
        if (nextPos == std::string::npos)
            nextPos = line.length() + 1;
        (*comps[i]) = std::stof(line.substr(lastPos + 1, nextPos - 1));
    }
}

inline Mesh* ImportASCII(const char* path, std::size_t fileSize)
{
    Mesh* mesh;
    std::ifstream is(path);

    if (is)
    {
        // Allocate a temporary array
        Triangle temp[fileSize / minSize];

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
                    ReadVec3(line, normalPos, temp[i].Normal);

                    // Read "outer loop"
                    if (!std::getline(is, line))
                    {
                        valid = false;
                        break;
                    }

                    Vec3* vecs [3] = { &temp[i].A, &temp[i].B, &temp[i].C };
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
                                ReadVec3(line, vertexPos, *(vecs[i]));
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

                    i++;
                }
            }
        }

        if (valid)
        {
            mesh = new Mesh(i);
            mesh->TrigsFromArray(temp);
            return mesh;
        }
        else
            throw std::runtime_error("The ASCII file is invalid");
    }
    else
        throw std::runtime_error(format_string("Could not open ASCII stl file with path: %s", path));

    is.close();

    return mesh;
}

Mesh* ImportSTL(const char *path)
{
    std::ifstream is(path, std::ifstream::binary);
    if (is)
    {
        // get length of file:
        is.seekg (0, is.end);
        std::size_t length = is.tellg();
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
            return ImportASCII(path, length);
        }
    }
    else
        throw std::runtime_error(format_string("Could not open stl file with path: %s", path));
}
