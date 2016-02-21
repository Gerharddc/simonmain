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

void AddPointToArray(float *array, Point &p, std::size_t arrPos)
{
    // We need to add the point twice because one goes up and one down
    array[arrPos + 0] = p.x; // up
    array[arrPos + 1] = p.y;
    array[arrPos + 2] = p.x; // down
    array[arrPos + 3] = p.y;
}

inline void AddPointsToArray(float *array, Point &p, std::size_t arrPos)
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

    short curIdx = 0;

    // TODO: we need to add a way to deal with travel moves

    for (Island isle : layer.islands)
    {
        short pCount = isle.printPoints.size();

        for (short j = 0; j < pCount; j++)
        {
            bool isLast = (j == pCount - 1);
            bool isFirst = (j == 0);

            // Get the wrapped points
            Point prevPoint = isFirst ? isle.printPoints[pCount - 1] : isle.printPoints[j - 1];
            Point curPoint = isle.printPoints[j];
            Point nextPoint = isLast ? isle.printPoints[0] : isle.printPoints[j + 1];

            // Add all the position components
            std::size_t arrPos = 2 * curIdx;
            AddPointsToArray(ld->curFloats, curPoint, arrPos);
            AddPointsToArray(ld->prevFloats, prevPoint, arrPos);
            AddPointsToArray(ld->nextFloats, nextPoint, arrPos);

            // Make the first point up and the next down
            arrPos = curIdx;
            // Positive = up, negative = down
            // abs < 0.6= 2nd point else = 1st point
            ld->sides[arrPos + 0] = 0.1f;
            ld->sides[arrPos + 1] = -0.1f;
            ld->sides[arrPos + 2] = 0.5f;
            ld->sides[arrPos + 3] = 1.0f;
            ld->sides[arrPos + 4] = -1.0f;

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
            arrPos = curIdx / 5 * 12; // TODO: opto

            // Connector trigs
            /*indices[arrPos + 0] = -1;
            indices[arrPos + 1] = -1;
            indices[arrPos + 2] = -1;
            indices[arrPos + 3] = -1;
            indices[arrPos + 4] = -1;
            indices[arrPos + 5] = -1;*/
            ld->indices[arrPos + 0] = curIdx + 0;
            ld->indices[arrPos + 1] = curIdx + 3;
            ld->indices[arrPos + 2] = curIdx + 2;
            ld->indices[arrPos + 3] = curIdx + 1;
            ld->indices[arrPos + 4] = curIdx + 2;
            ld->indices[arrPos + 5] = curIdx + 4;

            // Rectangle
            ld->indices[arrPos + 6] = curIdx + 3;
            ld->indices[arrPos + 7] = curIdx + 5;
            ld->indices[arrPos + 8] = curIdx + 4;
            ld->indices[arrPos + 9] = curIdx + 4;
            ld->indices[arrPos + 10] = curIdx + 5;
            ld->indices[arrPos + 11] = curIdx + 6;

            // Increment the current vertex index
            curIdx += 5;
        }

        // Add the first two parts of the first point for the last point to connect to
        std::size_t arrPos = 2 * curIdx; // TODO: opto
        Point prevPoint = layer.points[pCount - 1];
        Point curPoint = layer.points[0];
        Point nextPoint = layer.points[1];
        AddPointToArray(ld->curFloats, curPoint, arrPos);
        AddPointToArray(ld->prevFloats, prevPoint, arrPos);
        AddPointToArray(ld->nextFloats, nextPoint, arrPos);
        arrPos = curIdx;
        ld->sides[arrPos + 0] = 0.1f;
        ld->sides[arrPos + 1] = -0.1f;

        curIdx += 2; // TODO: maybe wrong
    }

    return ld;
}
