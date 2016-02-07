#include "structures.h"

Mesh::Mesh(std::size_t size)
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
        new (vertices + i) Vertex();
    vertArrCount = size * 3;
}

Mesh::~Mesh()
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

void Mesh::ShrinkVertices(std::size_t vertCount)
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

void Mesh::ShrinkTrigs(std::size_t newSize)
{
    // Reallocate the arrays with the new size
    trigCount = newSize;
    indices = (std::size_t*)realloc(indices, sizeof(std::size_t) * newSize * 3);
    trigs = (Triangle*)realloc(trigs, sizeof(Triangle) * newSize);
}

float *Mesh::getFlatVerts()
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

float *Mesh::getFlatNorms()
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

float *Toolpath::getLineVerts()
{
    auto count = getLineCount();

    if (lineVerts != nullptr)
        delete[] lineVerts;

    lineVerts = new float[count * 6];

    std::size_t i = 0;

    for (Layer *layer : layers)
    {
        auto pCount = layer->points.size();

        for (uint j = 0; j < pCount; j++)
        {
            Point prev = (j == 0) ? layer->points[pCount - 1] : layer->points[j - 1];
            lineVerts[i + 0] = prev.x;
            lineVerts[i + 1] = prev.y;
            lineVerts[i + 2] = layer->z;

            Point p = layer->points[j];
            lineVerts[i + 3] = p.x;
            lineVerts[i + 4] = p.y;
            lineVerts[i + 5] = layer->z;

            i += 6;
        }
    }

    return lineVerts;
}

void AddPointToArray(float *array, Point &p, std::size_t arrPos)
{
    // We need to add the point twice because one goes up and one down
    array[arrPos + 0] = p.x;
    array[arrPos + 1] = p.y;
    array[arrPos + 2] = p.x;
    array[arrPos + 3] = p.y;
}

inline void AddNanToArray(float *array, std::size_t arrPos)
{
    array[arrPos] = NAN;
    array[arrPos + 2] = NAN;
}

void Toolpath::CalculateVertices()
{
    // TODO: clean previous arrays

    auto count = getLineCount();

    // We need an up and a down version of each point
    curFloats   = new float[count * 8];
    nextFloats  = new float[count * 8];
    prevFloats  = new float[count * 8];
    sides       = new float[count * 4];
    indices     = new short[count * 6];
    zFloats     = new float[count * 4];

    short curIdx = 0;
    std::size_t idxPos = 2; // The first two vertices are indexed last

    for (Layer *layer : layers)
    {
        auto pCount = layer->points.size();

        for (uint j = 0; j < pCount; j++)
        {
            bool last = (j == pCount - 1);

            // Get the wrapped points
            Point prevPoint = (j == 0) ? layer->points[pCount - 1] : layer->points[j - 1];
            Point curPoint = layer->points[j];
            Point nextPoint = last ? layer->points[0] : layer->points[j + 1];

            // Add all the position components
            std::size_t arrPos = 8 * curIdx;
            AddPointToArray(curFloats, curPoint, arrPos);
            AddPointToArray(curFloats, curPoint, arrPos + 4);
            AddPointToArray(prevFloats, prevPoint, arrPos);
            AddNanToArray(prevFloats, arrPos + 4);
            AddNanToArray(nextFloats, arrPos);
            AddPointToArray(nextFloats, nextPoint, arrPos + 4);

            // Make the first point up and the next down
            arrPos = 4 * curIdx;
            sides[arrPos + 0] = 0.0f;
            sides[arrPos + 1] = 1.0f;
            sides[arrPos + 2] = 0.0f;
            sides[arrPos + 3] = 1.0f;

            // Write the z floats
            zFloats[arrPos + 0] = layer->z;
            zFloats[arrPos + 1] = layer->z;
            zFloats[arrPos + 2] = layer->z;
            zFloats[arrPos + 3] = layer->z;

            // We connect the current points with the following ones
            // 0---------2
            // -----------
            // 1---------3
            arrPos = 6 * curIdx;
            if (last)
            {
                indices[arrPos + 0] = idxPos + 0;
                indices[arrPos + 1] = idxPos + 1;
                indices[arrPos + 2] = 0;
                indices[arrPos + 3] = idxPos + 1;
                indices[arrPos + 4] = 1;
                indices[arrPos + 5] = 0;
            }
            else
            {
                indices[arrPos + 0] = idxPos + 0;
                indices[arrPos + 1] = idxPos + 1;
                indices[arrPos + 2] = idxPos + 2;
                indices[arrPos + 3] = idxPos + 1;
                indices[arrPos + 4] = idxPos + 3;
                indices[arrPos + 5] = idxPos + 2;
            }

            // Increment the current vertex index
            curIdx++;
            idxPos += 4;
        }
    }
}

float *Toolpath::getCurFloats()
{
    if (curFloats == nullptr)
        CalculateVertices();

    return curFloats;
}

float *Toolpath::getNextFloats()
{
    if (nextFloats == nullptr)
        CalculateVertices();

    return nextFloats;
}

float *Toolpath::getPrevFloats()
{
    if (prevFloats == nullptr)
        CalculateVertices();

    return prevFloats;
}

short *Toolpath::getIndices()
{
    if (indices == nullptr)
        CalculateVertices();

    return indices;
}

float *Toolpath::getSides()
{
    if (sides == nullptr)
        CalculateVertices();

    return sides;
}

float *Toolpath::getZFloats()
{
    if (zFloats == nullptr)
        CalculateVertices();

    return zFloats;
}
