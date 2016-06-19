#include "chopperengine.h"
#include "Misc/globalsettings.h"
#include "clipper.hpp"
#include <iostream>
#include <vector>
#include <map>
#include <limits>
#include <algorithm>

using namespace ChopperEngine;
using namespace ClipperLib;

std::size_t ChopperEngine::layerCount = 0;
Mesh* ChopperEngine::sliceMesh = nullptr;
static LogDelegate slicerLogger = nullptr;

// Scale double to ints with this factor
static double scaleFactor = 1000000.0;

void ChopperEngine::SlicerLog(std::string message)
{
    if (slicerLogger != nullptr)
        slicerLogger(message);

    std::cout << message << std::endl;
}

struct LineSegment
{
    IntPoint p1, p2;
    bool usedInPolygon = false;
    std::size_t trigIdx;

    LineSegment(IntPoint _p1, IntPoint _p2, std::size_t _trigIdx) :
        p1(_p1), p2(_p2), trigIdx(_trigIdx) {}

    void SwapPoints()
    {
        IntPoint temp = p1;
        p1 = p2;
        p2 = temp;
    }
};

enum SegmentType
{
    OutlineSegment,
    InfillSegment
};

struct LayerSegment
{
    Paths outlinePaths;
    SegmentType type;
    int segmentSpeed;

    LayerSegment(SegmentType _type) :
        type(_type) {}
};

struct LayerIsland
{
    Paths outlinePaths;
    std::vector<LayerSegment> segments;
};


struct LayerComponent
{
    std::vector<LineSegment> initialLineList;
    std::map<std::size_t, std::size_t> faceToLineIdxs;
    std::vector<LayerIsland> islandList;
    int layerSpeed = 100; //TODO
};

static inline void getTrigPointFloats(Triangle &trig, double *arr, uint8_t pnt)
{
    arr[0] = (double)sliceMesh->vertexFloats[trig.vertIdxs[0] * 3 + pnt];
    arr[1] = (double)sliceMesh->vertexFloats[trig.vertIdxs[1] * 3 + pnt];
    arr[2] = (double)sliceMesh->vertexFloats[trig.vertIdxs[2] * 3 + pnt];
}

static LayerComponent* layerComponents = nullptr;

static inline Vertex &VertAtIdx(std::size_t idx)
{
    return sliceMesh->vertices[idx];
}

static inline Triangle &TrigAtIdx(std::size_t idx)
{
    return sliceMesh->trigs[idx];
}

static inline void SliceTrigsToLayers()
{
    SlicerLog("Slicing triangles into layers");

    for (std::size_t i = 0; i < layerCount; i++)
    {
        double zPoint = (double)i * GlobalSettings::LayerHeight.Get();
        std::vector<LineSegment> &lineList = layerComponents[i].initialLineList;
        lineList.reserve(50); // Maybe a nice amount?

        for (std::size_t j = 0; j < sliceMesh->trigCount; j++)
        {
            Triangle &trig = sliceMesh->trigs[j];

            // Check if the triangle contains the current layer's z
            double z[3];
            getTrigPointFloats(trig, z, 2);
            double minZ = std::min(z[0], std::min(z[1], z[2]));
            double maxZ = std::max(z[0], std::max(z[1], z[2]));
            if (zPoint <= maxZ && zPoint >= minZ)
            {
                // Determine at which points the z is on the triangle

                // We need to determine which two sides of the triangle intersect with the z point
                // point A should be the point where these two sides connect
                // point B and C should therefore be the other two points
                uint8_t a, b, c;
                bool set = false;
                if (zPoint == z[0])
                {
                    if (zPoint == z[1])
                    {
                        a = 2;
                        b = 0;
                        c = 1;
                        set = true;
                    }
                    else if (zPoint == z[3])
                    {
                        a = 1;
                        b = 2;
                        c = 0;
                        set = true;
                    }
                }
                else if (zPoint == z[1] && zPoint == z[2])
                {
                    a = 0;
                    b = 1;
                    c = 2;
                    set = true;
                }

                if (!set)
                {
                    bool oneTwo = false;
                    bool oneThree = false;
                    bool twoThree = false;

                    if ((zPoint <= z[0] && zPoint >= z[1]) || (zPoint >= z[0] && zPoint <= z[1]))
                        oneTwo = true;
                    if ((zPoint <= z[0] && zPoint >= z[2]) || (zPoint >= z[0] && zPoint <= z[2]))
                        oneTwo = true;
                    if ((zPoint <= z[1] && zPoint >= z[2]) || (zPoint >= z[1] && zPoint <= z[3]))
                        oneTwo = true;

                    if (oneTwo && oneThree)
                    {
                        a = 0;
                        b = 1;
                        c = 2;
                    }
                    else if (oneTwo && twoThree)
                    {
                        a = 1;
                        b = 2;
                        c = 0;
                    }
                    else
                    {
                        a = 2;
                        b = 0;
                        c = 1;
                    }
                }

                double x[3], y[3];
                getTrigPointFloats(trig, x, 0);
                getTrigPointFloats(trig, y, 0);

                // First calculate the relationship of z to x on the one side of the triangle
                double zToX1 = (z[a] != z[b]) ? (x[a] - x[b] / z[a] - z[b]) : 0;
                // Then calculate the relationship of z to y on the one side of the triangle
                double zToY1 = (z[a]  != z[b]) ? (y[a] - y[b] / z[a] - z[b]) : 0;
                // Then calculate the relationship of z to x on the other side of the triangle
                double zToX2 = (z[a]  != z[c]) ? (x[a] - x[c] / z[a] - z[c]) : 0;
                // Then calculate the relationship of z to y on the other side of the triangle
                double zToY2 = (z[a]  != z[c]) ? (y[a] - y[c] / z[a] - z[c]) : 0;

                // Now calculate the z rise above pointB
                double zRise1 = zPoint - z[b];
                // And also the z rise above pointC
                double zRise2 = zPoint - z[c];

                // We can now calculatet the x and y points on both sides of the triangle using the z rises that were calculated above
                IntPoint p1 = IntPoint((cInt)(x[b] + zToX1 * zRise1 * scaleFactor),
                                       (cInt)(y[b] + zToY1 * zRise1 * scaleFactor));
                IntPoint p2 = IntPoint((cInt)(x[c] + zToX2 * zRise2 * scaleFactor),
                                       (cInt)(y[c] + zToY2 * zRise2 * scaleFactor));

                // Add the line and keep of track of which face it relates to
                layerComponents[i].faceToLineIdxs.insert(std::make_pair(j, lineList.size()));
                lineList.push_back(LineSegment(p1, p2, j));
            }
        }
    }
}

static inline cInt SquaredDist(IntPoint& p1, IntPoint& p2)
{
    return std::pow(p2.X - p1.X, 2) + std::pow(p2.Y - p1.Y, 2);
}

static void ProcessPolyNode(PolyNode &pNode, bool isHole, std::vector<LayerIsland> &isleList)
{
    // Non-hole nodes together with their child holes form the outlines of islands
    // but non-hole children form new islands
    if (!isHole)
    {
        isleList.emplace_back();
        LayerIsland &isle = isleList.back();
        isle.outlinePaths.push_back(pNode.Contour);

        for (PolyNode &cNode : pNode.Childs)
        {
            if (cNode.IsHole())
            {
                isle.outlinePaths.push_back(cNode.Contour);
                ProcessPolyNode(cNode, true, isleList);
            }

            else
                ProcessPolyNode(cNode, false, isleList);
        }
    }
    else
    {
        // A non-hole node should not be able to contain holes
        for (PolyNode &cNode : pNode.Childs)
        {
            ProcessPolyNode(cNode, false, isleList);
        }
    }
}

static inline void CalculateIslandsFromInitialLines()
{
    SlicerLog("Calculating initial islands");

    for (std::size_t i = 0; i < layerCount; i++)
    {
        SlicerLog("Calculating islands for layer: " + i);
        LayerComponent &layerComp = layerComponents[i];
        std::vector<LineSegment> &lineList = layerComp.initialLineList;

        if (lineList.size() < 2)
            continue;

        // We need a list of polygons which have already been closed and those that still need closing
        Paths closedPaths, openPaths;

        // i = startLine idx
        for (std::size_t i = 0; i < lineList.size(); i++)
        {
            //Check if the line has lready been used
            LineSegment &curLine = lineList[i];
            if (curLine.usedInPolygon)
                continue;

            Path path;
            path.push_back(curLine.p1);

            std::size_t lineIdx = i;
            bool closed = false;

            while (!closed)
            {
                lineList[lineIdx].usedInPolygon = true;
                IntPoint p1 = lineList[lineIdx].p2;
                path.push_back(p1);

                bool connected = false;

                for (uint8_t j = 0; j < 3; j++)
                {
                    Vertex &v = VertAtIdx(TrigAtIdx(curLine.trigIdx).vertIdxs[j]);

                    for (std::size_t touchIdx : v.trigIdxs)
                    {
                        // Check if this triangle has a linesegment on this layer
                        auto touchLineItr = layerComp.faceToLineIdxs.find(touchIdx);
                        if (touchLineItr == layerComp.faceToLineIdxs.end())
                            continue;
                        std::size_t touchLineIdx = touchLineItr->second;

                        LineSegment &touchLine = lineList[touchLineIdx];

                        const cInt minDiff = (cInt)(0.01 * 0.01 * scaleFactor * scaleFactor);

                        if (SquaredDist(p1, touchLine.p1) < minDiff)
                            connected = true;
                        else if (SquaredDist(p1, touchLine.p2) < minDiff)
                        {
                            connected = true;
                            touchLine.SwapPoints();
                        }

                        if (connected)
                        {
                            if (touchLineIdx == i)
                            {
                                closed = true;
                                break;
                            }

                            if (touchLine.usedInPolygon)
                                continue;

                            lineIdx = touchLineIdx;
                            break;
                        }
                    }

                    if (connected)
                        break;
                }

                if (!connected)
                    break;
            }

            if (closed)
                closedPaths.push_back(path);
            else
                openPaths.push_back(path);
        }

        // The list is no longer needed and can be removed to save memory
        lineList.clear();

        for (std::size_t j = 0; j < openPaths.size(); j++)
        {
            if (openPaths[j].size() < 1)
                continue;

            for (std::size_t k = 0; k < openPaths.size(); k++)
            {
                if (openPaths[k].size() < 1)
                    continue;

                IntPoint p1 = openPaths[j].back();
                IntPoint p2 = openPaths[k].front();

                const cInt minDiff = (cInt)(0.02 * 0.02 * scaleFactor * scaleFactor);
                if (SquaredDist(p1, p2) < minDiff)
                {
                    if (j == k)
                    {
                        closedPaths.push_back(openPaths[j]);
                        openPaths[j].clear();
                        break;
                    }
                    else
                    {
                        openPaths[j].insert(openPaths[j].begin(), openPaths[k].begin(), openPaths[k].end());
                        openPaths[k].clear();
                    }
                }
            }
        }

        // TODO: combine the below and the above, also possibly add hashing

        while (true)
        {
            long bestDist = std::numeric_limits<long>::max();
            long bestA = -1;
            long bestB = -1;
            bool wrongWay = false;

            for (std::size_t j = 0; j < openPaths.size(); j++)
            {
                if (openPaths[j].size() < 1)
                    continue;

                for (std::size_t k = 0; k < openPaths.size(); k++)
                {
                    if (openPaths[k].size() < 1)
                        continue;

                    IntPoint p1 = openPaths[j].back();
                    IntPoint p2 = openPaths[k].front();

                    long diff = SquaredDist(p1, p2);
                    if (diff < bestDist)
                    {
                        bestDist = diff;
                        bestA = j;
                        bestB = k;
                        wrongWay = false;
                    }

                    if (j != k)
                    {
                        p2 = openPaths[k].back();
                        diff = SquaredDist(p1, p2);

                        if (diff < bestDist)
                        {
                            bestDist = diff;
                            bestA = i;
                            bestB = j;
                            wrongWay = true;
                        }
                    }
                }
            }

            if (bestDist == std::numeric_limits<long>::max())
                break;

            if (bestA == bestB)
            {
                closedPaths.push_back(openPaths[bestA]);
                openPaths[bestA].clear();
            }
            else
            {
                if (wrongWay)
                    std::reverse(openPaths[bestA].begin(), openPaths[bestA].end());

                openPaths[bestA].insert(openPaths[bestA].begin(), openPaths[bestB].begin(), openPaths[bestB].end());
                openPaths[bestB].clear();
            }
        }

        // We now need to put the newly created polygons through clipper sothat it can detect holes for us
        // and then make proper islands with the returned data

        PolyTree resultTree;
        Clipper clipper;
        clipper.AddPaths(paths, PolyType::ptClip, true);
        clipper.Execute(ClipType::ctUnion, resultTree, PolyFillType::pftEvenOdd, PolyFillType::pftEvenOdd);

        // We need to itterate through the tree recursively because of its child structure
        for (PolyNode &pNode : resultTree.Childs)
            ProcessPolyNode(pNode, false, layerComp.islandList);

        // Optimize memory usage
        layerComp.islandList.shrink_to_fit();
    }
}

static inline void OptimizeOutlinePaths()
{
    for (LayerComponent layerComp : layerComponents)
    {
        for (LayerIsland isle : layerComp.islandList)
        {
            for (std::size_t i; i < isle.outlinePaths.size(); i++)
            {
                Path &path = isle.outlinePaths[i];
                Path optiPath;
                optiPath.reserve(path.size());

                // Go through each point and check if the next one is either
                // too close or part of an almost straight line
                std::size_t j = 0;
                while (j < (path.size() - 1))
                {
                    IntPoint p1 = path[j];

                    std::size_t k = j + 1;
                    IntPoint p2 = path[k];

                    // Skip past all the very close points
                    const cInt minDiff = (cInt)(0.01 * 0.01 * scaleFactor * scaleFactor);
                    while ((k < path.size()) && (SquaredDist(p1, p2) < minDiff))
                    {
                        k++;
                        p2 = path[k];
                    }

                    // Skip past points almost in line with their following
                    // and previous points
                    bool notLine = false;
                    while ((k < path.size()) && notLine)
                    {
                        IntPoint p3 = path[(k == path.size()) ? 0 : k + 1];

                        // We calculate the cosine between p2p1 and p2p3 to see if they
                        // are almost in a straight line

                        IntPoint A = p1 - p2;
                        IntPoint B = p3 - p2;
                        double dotP = A.X * A.X + B.Y * B.Y;
                        double magA = std::sqrt(A.X * A.X + A.Y * A.Y);
                        double magB = std::sqrt(B.X * B.X + B.Y * B.Y);

                        double cos = dotP / (magA * magB);

                        const double pi = 3.14159265358979323846;
                        const double thresh = std::cos(177.5 / 180.0 * pi);

                        // More negative cos is closer to 180 degrees
                        if (cos < thresh)
                        {
                            k++;
                            p2 = path[k];
                        }
                        else
                            notLine = true;
                    }

                    // We can now add the valid point and search for the next one
                    optiPath.push_back(p2);
                    j = k;
                }

                // Finally we can move to the optimized path
                optiPath.shrink_to_fit();
                isle.outlinePaths[i] = optiPath;
            }
        }
    }
}

static inline void GenerateOutlineSegments()
{
    SlicerLog("Generating outline segments");

    // Check if there should be at least one shell
    if (GlobalSettings::ShellThickness < 1)
        return;

    for (std::size_t i = 0; i < layerCount; i++)
    {
        LayerComponent &layerComp = layerComponents[i];

        SlicerLog("Outline: " + i);

        for (LayerIsland isle : layerComp.islandList)
        {
            // TODO: remove the invalid islands
            if (isle.outlinePaths.size() < 1)
                continue;

            const float NozzleWidth = 0.5f;
            cInt halfNozzle = -(NozzleWidth * scaleFactor / 2.0f);

            // The first outline will be one that is half an extrusion thinner
            // than the sliced outline, ths is sothat the dimensions
            // do not change once extruded
            Paths outline;
            ClipperOffset offset;
            offset.AddPaths(isle.outlinePaths, JoinType::jtMiter, EndType::etClosedPolygon);
            offset.Execute(outline, halfNozzle);

            for (std::size_t j = 0; j < GlobalSettings::ShellThickness; j++)
            {
                // Place the newly created outline in its own segment
                LayerSegment outlineSegment(SegmentType::OutlineSegment);
                outlineSegment.segmentSpeed = layerComp.layerSpeed;
                outlineSegment.outlinePaths = outline;
                isle.segments.push_back(outlineSegment);
                cInt dist = halfNozzle - (NozzleWidth * scaleFactor * (j + 1));

                //We now shrink the outline with one extrusion width for the next shell if any
                offset.Clear();
                offset.AddPaths(isle.outlinePaths, JoinType::jtMiter, EndType::etClosedPolygon);
                offset.Execute(outline, distance);
            }

            // We now need to store the smallest outline as the new layer outline for infill trimming purposes
            // the current outline though is just half an extrusion width to small
            offset.Clear();
            offset.AddPaths(outline, JoinType::jtMiter, EndType::etClosedPolygon);
            offset.Execute(outline, NozzleWidth * scaleFactor);
        }
    }
}

void ChopperEngine::SliceFile(Mesh *inputMesh, std::string outputFile)
{
    sliceMesh = inputMesh;

    // Calculate the amount layers that will be sliced
    layerCount = (std::size_t)(sliceMesh->MaxVec.z / GlobalSettings::LayerHeight.Get());

    if (layerComponents != nullptr)
        layerComponents = (LayerComponent*)realloc(layerComponents, sizeof(LayerComponent) * layerCount);
    else
        layerComponents = (LayerComponent*)malloc(sizeof(LayerComponent) * layerCount);

    for (std::size_t i = 0; i < layerCount; i++)
        layerComponents[i] = LayerComponent();

    // Slice the triangles into layers
    SliceTrigsToLayers();

    // Calculate islands from the original lines
    // Optimize the outline polygons
    OptimizeOutlinePaths();

    // Generate the outline segments
    GenerateOutlineSegments();

    // Generate the infill grids

    // The top and bottom segments need to calculated before
    // the infill outlines otherwise the infill will be seen as top or bottom
    // Calculate the top and bottom segments

    // Calculate the infill segments

    // Calculate the support segments

    // Combine the infill segments

    // Generate a raft

    // Generate a skirt

    // Calculate the toolpath

    // Free the memory
    if (layerComponents != nullptr)
        free(layerComponents);
}
