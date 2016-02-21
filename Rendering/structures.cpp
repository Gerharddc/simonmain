#include "structures.h"

#include <QDebug>

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

void AddPointToArray(float *array, Point2 &p, std::size_t arrPos)
{
    // We need to add the point twice because one goes up and one down
    array[arrPos + 0] = p.x; // up
    array[arrPos + 1] = p.y;
    array[arrPos + 2] = p.x; // down
    array[arrPos + 3] = p.y;
}

inline void AddPointsToArray(float *array, Point2 &p, std::size_t arrPos)
{
    // We add the point twice because one connects to the next and one
    // to the previous
    AddPointToArray(array, p, arrPos);
    AddPointToArray(array, p, arrPos + 4);
    array[arrPos + 8] = p.x;
    array[arrPos + 9] = p.y;
}

LayerData* Toolpath::CalculateLayerData(std::size_t layerNum)
{
    LayerData *ld = new LayerData();

    // Check that the layer is in range
    if ((layerNum + 1) > layers.size())
        return nullptr;

    Layer &layer = layers[layerNum];

    //auto count = layer.points.size();
    short pointCount = 0;

    for (Island isle : layer.islands)
        pointCount += isle.printPoints.size();

    short isleCount = layer.islands.size();
    ld->pointFloatCount = pointCount * 10 + (4 * isleCount);
    ld->sideFloatCount = pointCount * 5 + (2 * isleCount);
    ld->idxCount = pointCount * 12;

    // We need an up and a down version of each point
    ld->curFloats   = new float[ld->pointFloatCount];
    ld->nextFloats  = new float[ld->pointFloatCount];
    ld->prevFloats  = new float[ld->pointFloatCount];
    ld->sides       = new float[ld->sideFloatCount];
    ld->indices     = new short[ld->idxCount];

    short idxPos = 0;
    short saveIdx = 0;

    // TODO: we need to add a way to deal with travel moves

    for (Island isle : layer.islands)
    {
        short pCount = isle.printPoints.size();

        if (pCount < 2)
            continue;

        for (short j = 0; j < pCount; j++)
        {
            bool isLast = (j == pCount - 1);
            bool isFirst = (j == 0);

            // Get the wrapped points
            Point2 prevPoint = isFirst ? isle.printPoints[pCount - 1] : isle.printPoints[j - 1];
            Point2 curPoint = isle.printPoints[j];
            Point2 nextPoint = isLast ? isle.printPoints[0] : isle.printPoints[j + 1];

            // Add all the position components
            short pointPos = saveIdx * 2;
            AddPointsToArray(ld->curFloats, curPoint, pointPos);
            AddPointsToArray(ld->prevFloats, prevPoint, pointPos);
            AddPointsToArray(ld->nextFloats, nextPoint, pointPos);

            // Make the first point up and the next down
            // Positive = up, negative = down
            // abs < 0.6= 2nd point else = 1st point
            ld->sides[saveIdx + 0] = 0.1f;
            ld->sides[saveIdx + 1] = -0.1f;
            ld->sides[saveIdx + 2] = 0.5f;
            ld->sides[saveIdx + 3] = 1.0f;
            ld->sides[saveIdx + 4] = -1.0f;

            // We connect the current points with the following ones
            // The 4 current points form the corner and then we connect
            // to the next point to come
            // 0--2--4
            // | /| /|
            // |/ |/ |
            // 1--3--5
            // Only one of the corner triangles will be visible if any because
            // two points of the other will be the same when the rectangle
            // intersections are calculated

            // Connector trigs
            ld->indices[idxPos + 0] = saveIdx + 0;
            ld->indices[idxPos + 1] = saveIdx + 3;
            ld->indices[idxPos + 2] = saveIdx + 2;
            ld->indices[idxPos + 3] = saveIdx + 1;
            ld->indices[idxPos + 4] = saveIdx + 2;
            ld->indices[idxPos + 5] = saveIdx + 4;

            // Rectangle
            ld->indices[idxPos + 6] = saveIdx + 3;
            ld->indices[idxPos + 7] = saveIdx + 5;
            ld->indices[idxPos + 8] = saveIdx + 4;
            ld->indices[idxPos + 9] = saveIdx + 4;
            ld->indices[idxPos + 10] = saveIdx + 5;
            ld->indices[idxPos + 11] = saveIdx + 6;

            idxPos += 12;
            saveIdx += 5;
        }

        // Add the first two parts of the first point for the last point to connect to
        Point2 prevPoint = isle.printPoints[pCount - 1];
        Point2 curPoint = isle.printPoints[0];
        Point2 nextPoint = isle.printPoints[1];
        short pointPos = saveIdx * 2;
        AddPointToArray(ld->curFloats, curPoint, pointPos);
        AddPointToArray(ld->prevFloats, prevPoint, pointPos);
        AddPointToArray(ld->nextFloats, nextPoint, pointPos);
        ld->sides[saveIdx + 0] = 0.1f;
        ld->sides[saveIdx + 1] = -0.1f;

        saveIdx += 2;
    }

    return ld;
}
