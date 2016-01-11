#include "stlimporting.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <ios>
#include <math.h>
#include <unordered_map>

#include "Misc/strings.h"

// NOTE: float has to be 32bit real number
// TODO: add check

// The plan here to to allocate as few times as possible and to reuse allocated memory space as much as possilbe
// it is therefor needed to make excessive use of pointers and references.
// This needs to be thought through very well because this code has a large workload and needs to be of as high performance
// as possible whilst using as little memory as possible.

namespace STLImporting
{

// This function reads a float from a binary stream using a preallocated buffer
float ReadFloat(std::ifstream &is, char* buf)
{
    is.read(buf, 4);
    return *((float*) buf);
}

// This function reads a vertex into a float array
void ReadVec3(std::ifstream &is, char* buf, float *arr)
{
    arr[0] = ReadFloat(is, buf);
    arr[1] = ReadFloat(is, buf);
    arr[2] = ReadFloat(is, buf);
}

struct HashVertex
{
    // This functor creates a hash value from a vertex
    // NOTE: this has not been optimized for best spreading at all!!!
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
    // This functor compares to vertices
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

    // We allocate this buffer once and resuse it for all the floats that need to be read
    char buf[4];

    // Store a hashtable of the exisiting vertices to easily find indices of exisitng vertices
    std::unordered_map<float*, int, HashVertex, CompVertex> vecTable;

    uint16_t curIdx = 0; // The first open index in the vertex buffer
    uint16_t saveIdx = 0; // The current index that need to be saved

    for (unsigned int i = 0; i < trigCount; i++)
    {
        // Read the normal first
        //ReadVec3(is, buf, &(mesh->normalFloats[i * 3]));
        // Read past the normal
        is.seekg((std::size_t)is.tellg() + sizeof(float) * 3);

        // We then read the next 3 vertices to the current position in the vertex buffer. If they are all new then
        // they will be able to stay in these position. If they already exisit though then they will be overidden.
        // This should result in the least amount of memory operations.
        float* posV[3];
        posV[0] = &(mesh->vertexFloats[curIdx * 3]);
        posV[1] = &(mesh->vertexFloats[curIdx * 3 + 3]);
        posV[2] = &(mesh->vertexFloats[curIdx * 3 + 6]);

        // Read all 3 the vectors in
        ReadVec3(is, buf, posV[0]);
        ReadVec3(is, buf, posV[1]);
        ReadVec3(is, buf, posV[2]);

        // Cache a pointer to the current triangle for use by the vertex allocator
        Triangle* trig = &(mesh->trigs[i]);

        uint8_t fillPos = 0;
        for (uint8_t j = 0; j < 3; j++)
        {
            if (vecTable.count(posV[j]) == 0)
            {
                // We need to move the floats from new vertices to the front of the float array. In ther words,
                // we need to replace the read values with any exisiting vertices with those of new one.
                if (j != fillPos)
                    memcpy(posV[fillPos], posV[j], sizeof(float) * 3);
                vecTable.emplace(posV[fillPos], curIdx);
                mesh->indices[saveIdx] = curIdx;

                /*Vertex& vert = mesh->vertices[curIdx];
                vert.idx = curIdx;
                vert.trigs.push_back(trig);
                mesh->trigs[i].vertices[j] = &vert;*/

                curIdx++;
                fillPos++;

                // Update the min and max values for the mesh with this new vertex
                float* minComps[] = { &MinVec.x, &MinVec.y, &MinVec.z };
                float* maxComps[] = { &MaxVec.x, &MaxVec.y, &MaxVec.z };
                for (uint8_t k = 0; k < 3; k++)
                {
                    if (posV[j][k] < *(minComps[k]))
                        *(minComps[k]) = posV[j][k] ;

                    if (posV[j][k] > *(maxComps[k]))
                        *(maxComps[k]) = posV[j][k] ;
                }
            }
            else
            {
                // For existing vertices we reuse the indices of the first occurences of said verties
                auto pos = vecTable[posV[j]];
                mesh->indices[saveIdx] = pos;

                /*Vertex& vert = mesh->vertices[pos];
                vert.trigs.push_back(trig);
                mesh->trigs[i].vertices[j] = &vert;*/
            }
            saveIdx++;
        }

        // Skip past the empty two bytes at the end
        is.seekg((std::size_t)is.tellg() + 2);
    }

    is.close();

    mesh->MinVec = MinVec;
    mesh->MaxVec = MaxVec;
    mesh->vertexCount = curIdx; // Save the total amount of unique vertices

    return mesh;
}

void ReadVec3(std::string &line, std::size_t &lastPos, float* arr)
{
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
        arr[i] = val;
    }
}

// This is a rough (hopefully safe) estimation of the amount of bytes that an ASCII facet
// defination can take up and is used to calculate an overly large buffer
const uint minSize = 100;

inline Mesh* ImportASCII(const char* path, std::size_t fileSize)
{   
    // Allocate a large enough mesh and shrink it later
    Mesh* mesh = new Mesh(fileSize / minSize);
    std::ifstream is(path);

    // Check for a valid file
    if (is)
    {
        Vec3 MinVec, MaxVec;
        MinVec.ToMax();
        MaxVec.ToMin();

        std::string line;
        std::size_t i = 0;
        bool valid = true;

        // Store a hashtable of the exisiting vertices to easily find indices of exisitng vertices
        std::unordered_map<float*, int, HashVertex, CompVertex> vecTable;

        uint16_t curIdx = 0; // The first open index in the vertex buffer
        uint16_t saveIdx = 0; // The current index that need to be saved

        while (valid && std::getline(is, line))
        {
            if (line.find("facet") != std::string::npos)
            {
                // Cache a pointer to the current triangle for use by the vertex allocator
                Triangle *trig = &(mesh->trigs[i]);

                std::size_t normalPos = line.find("normal");
                if (normalPos == std::string::npos)
                    valid = false;
                else
                {
                    // Ignore the nromal
                    //ReadVec3(line, normalPos, &(mesh->normalFloats[i * 3]));

                    // Read "outer loop"
                    if (!std::getline(is, line))
                    {
                        valid = false;
                        break;
                    }

                    // We then read the next 3 vertices to the current position in the vertex buffer. If they are all new then
                    // they will be able to stay in these position. If they already exisit though then they will be overidden.
                    // This should result in the least amount of memory operations.
                    float* posV[3];
                    posV[0] = &(mesh->vertexFloats[curIdx * 3]);
                    posV[1] = &(mesh->vertexFloats[curIdx * 3 + 3]);
                    posV[2] = &(mesh->vertexFloats[curIdx * 3 + 6]);

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
                                ReadVec3(line, vertexPos, posV[j]);
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

                    // Determine which vertices are new and store them
                    uint8_t fillPos = 0;
                    for (uint8_t j = 0; j < 3; j++)
                    {
                        if (vecTable.count(posV[j]) == 0)
                        {
                            // We need to move the floats from new vertices to the front of the float array. In ther words,
                            // we need to replace the read values with any exisiting vertices with those of new one.
                            if (j != fillPos)
                                memcpy(posV[fillPos], posV[j], sizeof(float) * 3);
                            vecTable.emplace(posV[fillPos], curIdx);
                            mesh->indices[saveIdx] = curIdx;

                            /*Vertex *vert = &(mesh->vertices[curIdx]);
                            vert->idx = curIdx;
                            vert->trigs.push_back(trig);
                            trig->vertices[j] = vert;*/
                            mesh->vertices[curIdx].trigIdxs.push_back(i);
                            mesh->trigs[i].vertIdxs[j] = curIdx;

                            curIdx++;
                            fillPos++;

                            // Update the min and max values for the mesh with this new vertex
                            float* minComps[] = { &MinVec.x, &MinVec.y, &MinVec.z };
                            float* maxComps[] = { &MaxVec.x, &MaxVec.y, &MaxVec.z };
                            for (uint8_t k = 0; k < 3; k++)
                            {
                                if (posV[j][k] < *(minComps[k]))
                                    *(minComps[k]) = posV[j][k] ;

                                if (posV[j][k] > *(maxComps[k]))
                                    *(maxComps[k]) = posV[j][k] ;
                            }
                        }
                        else
                        {
                            // For existing vertices we reuse the indices of the first occurences of said verties
                            auto pos = vecTable[posV[j]];
                            mesh->indices[saveIdx] = pos;

                            // For some reason trying to used pointers as cache here causes the compiler
                            // to act very strange
                            /*mesh->vertices[pos].trigs.push_back(&(mesh->trigs[i]));
                            mesh->trigs[i].vertices[j] = &(mesh->vertices[pos]);*/
                            mesh->vertices[pos].trigIdxs.push_back(i);
                            mesh->trigs[i].vertIdxs[j] = pos;

                        }
                        saveIdx++;
                    }                    
                }

                i++;
            }
        }

        if (valid)
        {
            //mesh = new Mesh(i);
            //mesh->TrigsFromArray(temp);
            mesh->MinVec = MinVec;
            mesh->MaxVec = MaxVec;
            mesh->vertexCount = curIdx;
            mesh->Resize(i);
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
    if (!is.fail())
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

        Mesh *mesh;

        // check if the length is correct for binary
        if (length == (84 + (trigCount * 50)))
            mesh = ImportBinary(is, trigCount);
        else
        {
            is.close();
            mesh = ImportASCII(path, length);
        }

        // Calculate the non-unit normals for each face first
        glm::vec3 faceNorms[mesh->trigCount];
        for (std::size_t i = 0; i < mesh->trigCount; i++)
        {
            glm::vec3 vecs[3];

            for (uint8_t j = 0; j < 3; j++)
            {
                auto idx = (mesh->indices[i + j]) * 3;
                vecs[j].x = mesh->vertexFloats[idx];
                vecs[j].y = mesh->vertexFloats[idx + 1];
                vecs[j].z = mesh->vertexFloats[idx + 2];
            }

            glm::vec3 e1 = vecs[0] - vecs[1];
            glm::vec3 e2 = vecs[2] - vecs[1];
            faceNorms[i] = glm::cross(e1, e2);
        }

        // Now we need to sum the face normals for each vertex with larger faces carying more weight
        // due to their non-unit normals. We can then normalize the sum to get the vertex normals.
        for (std::size_t i = 0; i < mesh->vertexCount; i++)
        {
            glm::vec3 norm(0.0f);

            for (std::size_t j = 0; j < mesh->vertices[i].trigIdxs.size(); j++)
            {
                norm += faceNorms[mesh->vertices[i].trigIdxs[j]];
            }

            mesh->vertices[i].trigIdxs.shrink_to_fit();

            if (norm != glm::vec3(0.0f))
                norm = glm::normalize(norm);

            std::size_t pos = i * 3;
            mesh->normalFloats[pos]       = norm.x;
            mesh->normalFloats[pos + 1]   = norm.y;
            mesh->normalFloats[pos + 2]   = norm.z;
        }

        return mesh;
    }
    else
        throw std::runtime_error(format_string("Could not open stl file with path: %s", path));
}

}
