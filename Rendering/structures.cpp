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

inline void AddPointsToArray(float *array, Point2 &p, short count, std::size_t &arrPos)
{
    for (short i = 0; i < count; i++)
    {
        array[arrPos + 0] = p.x;
        array[arrPos + 1] = p.y;
        arrPos += 2;
    }
}

inline void AddPointZsToArray(float *array, Point2 &p, float z, short count, std::size_t &arrPos)
{
    for (short i = 0; i < count; i++)
    {
        array[arrPos + 0] = p.x;
        array[arrPos + 1] = p.y;
        array[arrPos + 2] = z;
        arrPos += 3;
    }
}

// An GLES chunck can have a maximum of 2^16(ushort) indices and we need to divide all the data between that
// we will do this by creating a maximum size buffer for each set of data here and then shrinking it to only the
// needed part
const ushort maxIdx = 65536;

inline void NewChunck(ushort &idxPos, ushort &saveIdx, std::vector<TPDataChuck> *chunks, TPDataChuck *dc)
{
    // We need to shrink the previous chunck to size
    if (dc != nullptr)
    {
        //dc->pointFloatCount = saveIdx * 3;
        //dc->curFloatCount
    }

    idxPos = 0;
    saveIdx = 0;
    chunks->emplace_back();
    dc = chunks->back();

    dc->curFloats  = new float[maxIdx * 15];
    dc->nextFloats = new float[maxIdx * 10];
    dc->prevFloats = new float[maxIdx * 10];
    dc->sides      = new ushort[maxIdx * 5];
    dc->indices    = new float[maxIdx * 12];
}

std::vector<TPDataChuck>* Toolpath::CalculateDataChuncks()
{
    ushort idxPos = 0;
    ushort saveIdx = 0;
    std::vector<TPDataChuck> *chunks = new std::vector<TPDataChuck>();
    TPDataChuck *dc = nullptr;

    // Use a MACRO to easily push a new chunck
    #define NEWCHUNK NewChunck(idxPos, saveIdx, chunks, dc)
    NEWCHUNK;

    for (Layer layer : layers)
    {
        for (Island isle : layer.islands)
        {
            // We need to determine the maximum amount of points on the island in order to know if we
            // will have to cut it up into pieces
            bool cut = true;
            auto pCount = isle.printPoints.size();

            // Determine how many points we can still fit in this chunck
            int fitCount = (maxIdx - saveIdx - 2) / 5;
            int leftCount = 0;

#define MAXFITCOUNT fitCount = (maxIdx - 2) / 5

            // We need to fit at least 3 points
            if (fitCount < 3)
            {
                NEWCHUNK;
                MAXFITCOUNT;
            }

            if (pCount < fitCount)
            {
                cut = false;
                //fitCount = pCount;
            }
            else
            {
                cut = true;
                leftCount = pCount - fitCount;
            }

            auto fitPos = 0;
            for (auto j = 0; j < pCount; j++)
            {
                // Determine the connecting points
                bool isLast = (j == pCount - 1);
                bool isFirst = (j == 0);

                Point2 curPoint = isle.printPoints[j];
                Point2 prevPoint, nextPoint;
                bool hasNoPrev = false;
                bool hasNoNext = false;

                // Last or first points of open ended shapes need to be treated differently
                // those of closed-ended shapes
                // It is not possible to place the last point alone on the next chunk so we need to ensure it is never needed
                if (isLast)
                {
                    prevPoint = isle.printPoints[j - 1];
                    nextPoint = isle.printPoints[0];

                    if (curPoint == nextPoint)
                    {
                        // Add the first two parts of the first point for the last point to connect to
                        nextPoint = isle.printPoints[1];
                        AddPointZsToArray(dc->curFloats, curPoint, layer.z, 2, dc->curFloatCount);
                        AddPointsToArray(dc->prevFloats, prevPoint, 2, dc->prevFloatCount);
                        AddPointsToArray(dc->nextFloats, nextPoint, 2, dc->nextFloatCount);
                        dc->sides[saveIdx + 0] = 10.0f;
                        dc->sides[saveIdx + 1] = -10.0f;
                        saveIdx += 2;

                        continue;
                    }
                    else
                        hasNoNext = true;
                }
                else
                {
                    nextPoint = isle.printPoints[j + 1];

                    if (isFirst)
                    {
                        prevPoint = isle.printPoints[pCount - 1];

                        if (prevPoint == curPoint)
                            prevPoint = isle.printPoints[pCount - 2]; // TODO: error maybe?
                        else
                            hasNoPrev = true;
                    }
                    else
                        prevPoint = isle.printPoints[j - 1];

                    // Check if we need to move to a new chunck
                    if (cut)
                    {
                        if (fitPos > (fitCount - 2))
                        {
                            // If this point needs to go to a new chunk then we need to add its first 2
                            // vertices for the last one in this chunck to connect to
                            AddPointZsToArray(dc->curFloats, curPoint, layer.z, 2, dc->curFloatCount);
                            AddPointsToArray(dc->prevFloats, prevPoint, 2, dc->prevFloatCount);
                            AddPointsToArray(dc->nextFloats, nextPoint, 2, dc->nextFloatCount);
                            dc->sides[saveIdx + 0] = 10.0f;
                            dc->sides[saveIdx + 1] = -10.0f;
                            saveIdx += 2;

                            // We then need to move to a new chucnk
                            NEWCHUNK;

                            // We also need to determine if it will have to be cut again
                            MAXFITCOUNT;
                            if (leftCount < fitCount)
                                cut = false;
                            else
                            {
                                leftCount -= fitCount;
                                fitPos = 0;
                            }
                        }
                        else
                            fitPos++;
                    }
                }

                // We connect the current points with the following ones
                // The 4 current points form the corner and then we connect
                // to the next point to come
                // Only one of the corner triangles will be visible if any because
                // two points of the other will be the same when the rectangle
                // intersections are calculated

                if (hasNoPrev)
                {
                    // Add only 2 points
                    AddPointZsToArray(dc->curFloats, curPoint, layer.z, 2, dc->curFloatCount);
                    AddPointsToArray(dc->prevFloats, prevPoint, 2, dc->prevFloatCount);
                    AddPointsToArray(dc->nextFloats, nextPoint, 2, dc->nextFloatCount);

                    // Point attributes
                    // Forwards only
                    dc->sides[saveIdx + 0] = 40.0f;
                    dc->sides[saveIdx + 1] = -40.0f;

                    // Rectangle only
                    dc->indices[idxPos + 0] = saveIdx + 0;
                    dc->indices[idxPos + 1] = saveIdx + 2;
                    dc->indices[idxPos + 2] = saveIdx + 1;
                    dc->indices[idxPos + 3] = saveIdx + 2;
                    dc->indices[idxPos + 4] = saveIdx + 3;
                    dc->indices[idxPos + 5] = saveIdx + 1;

                    idxPos += 6;
                    saveIdx += 2;
                }
                else if (hasNoNext)
                {
                    // Add only 2 points
                    AddPointZsToArray(dc->curFloats, curPoint, layer.z, 2, dc->curFloatCount);
                    AddPointsToArray(dc->prevFloats, prevPoint, 2, dc->prevFloatCount);
                    AddPointsToArray(dc->nextFloats, nextPoint, 2, dc->nextFloatCount);

                    // Point attributes
                    // Backwards only
                    dc->sides[saveIdx + 0] = 50.0f;
                    dc->sides[saveIdx + 1] = -50.0f;

                    // Nothing for the indices

                    saveIdx += 2;
                }
                else
                {
                    // Add all the position components
                    AddPointZsToArray(dc->curFloats, curPoint, layer.z, 5, dc->curFloatCount);
                    AddPointsToArray(dc->prevFloats, prevPoint, 5, dc->prevFloatCount);
                    AddPointsToArray(dc->nextFloats, nextPoint, 5, dc->nextFloatCount);

                    // Point attributes
                    // Backwards, centre, forwards
                    dc->sides[saveIdx + 0] = 10.0f;
                    dc->sides[saveIdx + 1] = -10.0f;
                    dc->sides[saveIdx + 2] = 20.0f;
                    dc->sides[saveIdx + 3] = 30.0f;
                    dc->sides[saveIdx + 4] = -30.0f;

                    // Connector trigs
                    dc->indices[idxPos + 0] = saveIdx + 0;
                    dc->indices[idxPos + 1] = saveIdx + 3;
                    dc->indices[idxPos + 2] = saveIdx + 2;
                    dc->indices[idxPos + 3] = saveIdx + 1;
                    dc->indices[idxPos + 4] = saveIdx + 2;
                    dc->indices[idxPos + 5] = saveIdx + 4;

                    // Rectangle
                    dc->indices[idxPos + 6] = saveIdx + 3;
                    dc->indices[idxPos + 7] = saveIdx + 5;
                    dc->indices[idxPos + 8] = saveIdx + 4;
                    dc->indices[idxPos + 9] = saveIdx + 4;
                    dc->indices[idxPos + 10] = saveIdx + 5;
                    dc->indices[idxPos + 11] = saveIdx + 6;

                    idxPos += 12;
                    saveIdx += 5;
                }
            }
        }
    }
}


LayerData* Toolpath::CalculateLayerData(std::size_t layerNum)
{
    LayerData *ld = new LayerData();

    // Check that the layer is in range
    if ((layerNum + 1) > layers.size())
        return nullptr;

    Layer &layer = layers[layerNum];

    short pointCount = 0;
    short travelCount = 0;

    for (Island isle : layer.islands)
    {
        pointCount += isle.printPoints.size(); // TODO: hmmmm
        travelCount += isle.movePoints.size();
    }

    short isleCount = layer.islands.size();
    ld->pointFloatCount = pointCount * 10 + (4 * isleCount);
    ld->curFloatCount = ld->pointFloatCount * 1.5f; // The cur float adds the z value
    ld->sideFloatCount = pointCount * 5 + (2 * isleCount);
    ld->idxCount = pointCount * 12;
    ld->travelCount = (travelCount + isleCount) * 3;

    // We need an up and a down version of each point
    ld->curFloats    = new float[ld->curFloatCount];
    ld->nextFloats   = new float[ld->pointFloatCount];
    ld->prevFloats   = new float[ld->pointFloatCount];
    ld->sides        = new float[ld->sideFloatCount];
    ld->indices      = new short[ld->idxCount];
    ld->travelFloats = new float[ld->travelCount];

    short idxPos = 0;
    short saveIdx = 0;
    //short lineIdx = 0;
    std::size_t curPos = 0;
    std::size_t prevPos = 0;
    std::size_t nextPos = 0;

    // TODO: we need to add a way to deal with travel moves

    for (Island isle : layer.islands)
    {
        //isle.printPoints.pop_back();
        short pCount = isle.printPoints.size();

        if (pCount < 2)
            continue;

        for (short j = 0; j < pCount; j++)
        {
            bool isLast = (j == pCount - 1);
            bool isFirst = (j == 0);

            Point2 curPoint = isle.printPoints[j];
            Point2 prevPoint, nextPoint;
            bool hasNoPrev = false;
            bool hasNoNext = false;

            // Last or first points of open ended shapes need to be treated differently
            // those of closed-ended shapes
            if (isLast)
            {
                prevPoint = isle.printPoints[j - 1];
                nextPoint = isle.printPoints[0];

                if (curPoint == nextPoint)
                {
                    // Add the first two parts of the first point for the last point to connect to
                    nextPoint = isle.printPoints[1];
                    AddPointZsToArray(ld->curFloats, curPoint, layer.z, 2, curPos);
                    AddPointsToArray(ld->prevFloats, prevPoint, 2, prevPos);
                    AddPointsToArray(ld->nextFloats, nextPoint, 2, nextPos);
                    ld->sides[saveIdx + 0] = 10.0f;
                    ld->sides[saveIdx + 1] = -10.0f;

                    saveIdx += 2;

                    continue;
                }
                else
                    hasNoNext = true;
            }
            else
            {
                nextPoint = isle.printPoints[j + 1];

                if (isFirst)
                {
                    prevPoint = isle.printPoints[pCount - 1];

                    if (prevPoint == curPoint)
                        prevPoint = isle.printPoints[pCount - 2]; // TODO: error maybe?
                    else
                        hasNoPrev = true;
                }
                else
                    prevPoint = isle.printPoints[j - 1];
            }

            // We connect the current points with the following ones
            // The 4 current points form the corner and then we connect
            // to the next point to come
            // Only one of the corner triangles will be visible if any because
            // two points of the other will be the same when the rectangle
            // intersections are calculated

            if (hasNoPrev)
            {
                // Add only 2 points
                AddPointZsToArray(ld->curFloats, curPoint, layer.z, 2, curPos);
                AddPointsToArray(ld->prevFloats, prevPoint, 2, prevPos);
                AddPointsToArray(ld->nextFloats, nextPoint, 2, nextPos);

                // Point attributes
                // Forwards only
                ld->sides[saveIdx + 0] = 40.0f;
                ld->sides[saveIdx + 1] = -40.0f;

                // Rectangle only
                ld->indices[idxPos + 0] = saveIdx + 0;
                ld->indices[idxPos + 1] = saveIdx + 2;
                ld->indices[idxPos + 2] = saveIdx + 1;
                ld->indices[idxPos + 3] = saveIdx + 2;
                ld->indices[idxPos + 4] = saveIdx + 3;
                ld->indices[idxPos + 5] = saveIdx + 1;

                idxPos += 6;
                saveIdx += 2;
            }
            else if (hasNoNext)
            {
                // Add only 2 points
                AddPointZsToArray(ld->curFloats, curPoint, layer.z, 2, curPos);
                AddPointsToArray(ld->prevFloats, prevPoint, 2, prevPos);
                AddPointsToArray(ld->nextFloats, nextPoint, 2, nextPos);

                // Point attributes
                // Backwards only
                ld->sides[saveIdx + 0] = 50.0f;
                ld->sides[saveIdx + 1] = -50.0f;

                // Nothing for the indices

                saveIdx += 2;
            }
            else
            {
                // Add all the position components
                AddPointZsToArray(ld->curFloats, curPoint, layer.z, 5, curPos);
                AddPointsToArray(ld->prevFloats, prevPoint, 5, prevPos);
                AddPointsToArray(ld->nextFloats, nextPoint, 5, nextPos);

                // Point attributes
                // Backwards, centre, forwards
                ld->sides[saveIdx + 0] = 10.0f;
                ld->sides[saveIdx + 1] = -10.0f;
                ld->sides[saveIdx + 2] = 20.0f;
                ld->sides[saveIdx + 3] = 30.0f;
                ld->sides[saveIdx + 4] = -30.0f;

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
        }

        // Add the first two parts of the first point for the last point to connect to
        /*Point2 prevPoint = isle.printPoints[pCount - 1];
        Point2 curPoint = isle.printPoints[0];
        Point2 nextPoint = isle.printPoints[1];
        AddPointZsToArray(ld->curFloats, curPoint, layer.z, 2, curPos);
        AddPointsToArray(ld->prevFloats, prevPoint, 2, prevPos);
        AddPointsToArray(ld->nextFloats, nextPoint, 2, nextPos);
        ld->sides[saveIdx + 0] = 0.1f;
        ld->sides[saveIdx + 1] = -0.1f;

        saveIdx += 2;

        /*short tCount = isle.movePoints.size();

        if (tCount > 2)
        {
            for (short j = 0; j < tCount; j++)
            {
                Point3 mp = isle.movePoints[j];
                ld->travelFloats[lineIdx + 0] = mp.x;
                ld->travelFloats[lineIdx + 1] = mp.y;
                ld->travelFloats[lineIdx + 2] = mp.z;
                lineIdx += 3;
            }

            ld->travelFloats[lineIdx + 0] = -1;
            ld->travelFloats[lineIdx + 1] = -1;
            ld->travelFloats[lineIdx + 2] = -1;
            lineIdx += 3;
        }*/
    }

    return ld;
}
