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
const double PI = 3.14159265358979323846;

void ChopperEngine::SlicerLog(std::string message)
{
    if (slicerLogger != nullptr)
        slicerLogger(message);

    std::cout << message << std::endl;
}

struct TrigLineSegment
{
    // This is a linesegment that is linked to a triangle face

    IntPoint p1, p2;
    bool usedInPolygon = false;
    std::size_t trigIdx;

    TrigLineSegment(IntPoint _p1, IntPoint _p2, std::size_t _trigIdx) :
        p1(_p1), p2(_p2), trigIdx(_trigIdx) {}

    void SwapPoints()
    {
        IntPoint temp = p1;
        p1 = p2;
        p2 = temp;
    }
};

struct LineSegment
{
    IntPoint p1, p2;

    LineSegment(IntPoint _p1, IntPoint _p2) :
        p1(_p1), p2(_p2) {}
};

/*struct FloatLineSegment
{
    Point2 p1, p2;

    FloatLineSegment(Point2 _p1, Point2 _p2) :
        p1(_p1), p2(_p2) {}

    FloatLineSegment(IntPoint _p1, IntPoint _p2)
    {
        p1 = Point2((float)((double)_p1.X / scaleFactor), (float)((double)_p1.Y / scaleFactor));
        p1 = Point2((float)((double)_p2.X / scaleFactor), (float)((double)_p2.Y / scaleFactor));
    }
};*/

enum SegmentType
{
    OutlineSegment,
    InfillSegment,
    TopSegment,
    BottomSegment,
    SupportSegment,
    SkirtSegment,
    RaftSegment
};

enum ToolSegType
{
    Retraction,
    Moved,
    Extruded
};

struct ToolSegment
{
    ToolSegType type;
};

struct RetractSegment : public ToolSegment
{
    cInt distance;
};

// TODO: create specialized segment types to save memory
struct LayerSegment
{
    Paths outlinePaths;
    SegmentType type;
    int segmentSpeed;
    float infillMultiplier = 1.0f;
    std::vector<LineSegment> fillLines;
    float fillDensity = 1.0;
    std::vector<ToolSegment> toolSegments;

    LayerSegment(SegmentType _type) :
        type(_type) {}
};

struct LayerIsland
{
    Paths outlinePaths;
    std::vector<LayerSegment> segments;
};

struct IntPoint3
{
    cInt X, Y, Z;

    IntPoint3(cInt x, cInt y, cInt z) :
        X(x), Y(y), Z(z) {}
};

struct MoveSegment
{
    IntPoint3 p1, p2;
    int speed;

    MoveSegment(IntPoint3 _p1, IntPoint3 _p2, int _speed) :
        p1(_p1), p2(_p2), speed(_speed) {}
};

struct LayerComponent
{
    std::vector<TrigLineSegment> initialLineList;
    std::map<std::size_t, std::size_t> faceToLineIdxs;
    std::vector<LayerIsland> islandList;
    int layerSpeed = 100; //TODO
    std::vector<MoveSegment> initialLayerMoves;
};

struct InfillGrid
{
    Paths leftList, rightList;
};

// TODO: store multiple grids
static InfillGrid mainGrid;

static std::map<float, InfillGrid> InfillGridMap;

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


// TODO: reduce allocation of items to vectors, rather emplaceback


static inline void SliceTrigsToLayers()
{
    SlicerLog("Slicing triangles into layers");

    for (std::size_t i = 0; i < layerCount; i++)
    {
        double zPoint = (double)i * GlobalSettings::LayerHeight.Get();
        std::vector<TrigLineSegment> &lineList = layerComponents[i].initialLineList;
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
                lineList.push_back(TrigLineSegment(p1, p2, j));
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

        for (PolyNode *cNode : pNode.Childs)
        {
            if (cNode->IsHole())
            {
                isle.outlinePaths.push_back(cNode->Contour);
                ProcessPolyNode(*cNode, true, isleList);
            }

            else
                ProcessPolyNode(*cNode, false, isleList);
        }
    }
    else
    {
        // A non-hole node should not be able to contain holes
        for (PolyNode *cNode : pNode.Childs)
        {
            ProcessPolyNode(*cNode, false, isleList);
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
        std::vector<TrigLineSegment> &lineList = layerComp.initialLineList;

        if (lineList.size() < 2)
            continue;

        // We need a list of polygons which have already been closed and those that still need closing
        Paths closedPaths, openPaths;

        // i = startLine idx
        for (std::size_t i = 0; i < lineList.size(); i++)
        {
            //Check if the line has lready been used
            TrigLineSegment &curLine = lineList[i];
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

                        TrigLineSegment &touchLine = lineList[touchLineIdx];

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
                        openPaths[j].reserve(openPaths[j].size() + openPaths[k].size());
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

                openPaths[bestA].reserve(openPaths[bestA].size() + openPaths[bestB].size());
                openPaths[bestA].insert(openPaths[bestA].begin(), openPaths[bestB].begin(), openPaths[bestB].end());
                openPaths[bestB].clear();
            }
        }

        // We now need to put the newly created polygons through clipper sothat it can detect holes for us
        // and then make proper islands with the returned data

        PolyTree resultTree;
        Clipper clipper;
        clipper.AddPaths(closedPaths, PolyType::ptClip, true);
        clipper.Execute(ClipType::ctUnion, resultTree, PolyFillType::pftEvenOdd, PolyFillType::pftEvenOdd);

        // We need to itterate through the tree recursively because of its child structure
        for (PolyNode *pNode : resultTree.Childs)
            ProcessPolyNode(*pNode, false, layerComp.islandList);

        // Optimize memory usage
        layerComp.islandList.shrink_to_fit();
    }
}

static IntPoint operator-(const IntPoint &p1, const IntPoint &p2)
{
    return IntPoint(p1.X - p2.X, p1.Y - p2.Y);
}

static inline void OptimizeOutlinePaths()
{
    for (std::size_t a = 0; a < layerCount; a++)
    {
        LayerComponent &layerComp = layerComponents[a];

        for (LayerIsland &isle : layerComp.islandList)
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
                        const double thresh = std::cos(177.5 / 180.0 * PI);

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

const float NozzleWidth = 0.5f;

static inline void GenerateOutlineSegments()
{
    SlicerLog("Generating outline segments");

    // Check if there should be at least one shell
    if (GlobalSettings::ShellThickness.Get() < 1)
        return;

    for (std::size_t i = 0; i < layerCount; i++)
    {
        LayerComponent &layerComp = layerComponents[i];

        SlicerLog("Outline: " + i);

        for (LayerIsland &isle : layerComp.islandList)
        {
            // TODO: remove the invalid islands
            if (isle.outlinePaths.size() < 1)
                continue;

            cInt halfNozzle = -(NozzleWidth * scaleFactor / 2.0f);

            // The first outline will be one that is half an extrusion thinner
            // than the sliced outline, ths is sothat the dimensions
            // do not change once extruded
            Paths outline;
            ClipperOffset offset;
            offset.AddPaths(isle.outlinePaths, JoinType::jtMiter, EndType::etClosedPolygon);
            offset.Execute(outline, halfNozzle);

            for (std::size_t j = 0; j < GlobalSettings::ShellThickness.Get(); j++)
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
                offset.Execute(outline, dist);
            }

            // We now need to store the smallest outline as the new layer outline for infill trimming purposes
            // the current outline though is just half an extrusion width to small
            offset.Clear();
            offset.AddPaths(outline, JoinType::jtMiter, EndType::etClosedPolygon);
            offset.Execute(outline, NozzleWidth * scaleFactor);
        }
    }
}

static inline void GenerateInfillGrid(float density, float angle = 45.0f / 180.0f * PI)
{
    // TODO: get blank constructor working
    InfillGridMap.emplace(std::make_pair(density, InfillGrid()));
    InfillGrid &grid = InfillGridMap[density];
    Paths &rightList = grid.rightList;
    Paths &leftList = grid.leftList;

    // Calculate the needed spacing

    // d% = 1 / (x% + 1)
    // d% * x + d% = 1
    // a + d% = 1
    // a = 1 - d%
    // x = a / d%

    float a = 1 - density;
    float x = a / density;
    uint spacing = (uint)(NozzleWidth * scaleFactor * x);
    uint divider = spacing + (NozzleWidth * scaleFactor);

    // 2 points of linesegment
    IntPoint p1, p2;

    // We need to start creating diagonal lines before
    // the min x sothat there are lines over every part of the model
    cInt xOffset = (cInt)((sliceMesh->MaxVec.y - sliceMesh->MinVec.y) * scaleFactor / std::tan(angle));
    cInt modMinX = sliceMesh->MinVec.x - xOffset;

    std::size_t amountOfLines = (std::size_t)((sliceMesh->MaxVec.x * scaleFactor - modMinX) / divider);

    // Calculate the right and left line simultaneously
    for (std::size_t i = 0; i < amountOfLines; i++)
    {
        // First line angled to the right
        p1.X = modMinX + i * divider + NozzleWidth * scaleFactor / 2;
        p1.Y = sliceMesh->MinVec.y;
        p2.X = p1.X + xOffset;
        p2.Y = sliceMesh->MaxVec.y;

        // We make use of duplicated points so that the below changes do not affect this line
        Path line;
        line.push_back(p1);
        line.push_back(p2);
        line.shrink_to_fit();
        rightList.push_back(line);

        p2.Y = sliceMesh->MinVec.y;
        p1.Y = sliceMesh->MaxVec.y;

        line.clear();
        line.push_back(p1);
        line.push_back(p2);
        line.shrink_to_fit();
        leftList.push_back(line);
    }

    // Optimize the grids
    leftList.shrink_to_fit();
    rightList.shrink_to_fit();
}

static inline void GenerateInfillGrids()
{
    // Figure out what densities we need
    // TODO:

    InfillGridMap.clear();

    GenerateInfillGrid(15.0f);
    GenerateInfillGrid(100.0f);
}

static inline void CalculateTopBottomSegments()
{
    SlicerLog("Calculating top and bottom segments");

    // TODO: implement seperate top and bottom thickness
    Clipper clipper;

    // To calculate the top segments we need to go from the bottom up,
    // take each island as a subject, take the outline of the above layer
    // as a clipper and perform a difference operation. The result will
    // then be the top segment(s) for each layer

    // Go from the second layer to the second highest layer as everything
    // on the top layer is a top segment in any case
    // and everything on the first layer is a bottom segment in any case

    if (GlobalSettings::TopBottomThickness.Get() > 0) // TODO: top
    {
        for (std::size_t i = 1; i < layerCount - GlobalSettings::TopBottomThickness.Get(); i++)
        {
            //First we need to calculate the intersection of the top few layers above it
            Paths aboveIntersection;

            for (std::size_t j = i + 1; j < i + GlobalSettings::TopBottomThickness.Get() + 1; j++)
            {
                Paths combinedIsles;

                // Combine the outlines of the islands into one
                for (LayerIsland &isle : layerComponents[j].islandList)
                {
                    clipper.Clear();
                    clipper.AddPaths(combinedIsles, PolyType::ptClip, true);
                    clipper.AddPaths(isle.outlinePaths, PolyType::ptSubject, true);
                    clipper.Execute(ClipType::ctUnion, combinedIsles);
                }

                // Set the intial top if still unset
                if (aboveIntersection.size() < 1)
                {
                    aboveIntersection = combinedIsles;
                    continue;
                }

                clipper.Clear();
                clipper.AddPaths(aboveIntersection, PolyType::ptSubject, true);
                clipper.AddPaths(combinedIsles, PolyType::ptClip, true);
                clipper.Execute(ClipType::ctIntersection, aboveIntersection);
            }

            for (LayerIsland &isle : layerComponents[i].islandList)
            {
                LayerSegment topSegment(SegmentType::TopSegment);
                clipper.Clear();
                clipper.AddPaths(isle.outlinePaths, PolyType::ptSubject, true);
                clipper.AddPaths(aboveIntersection, PolyType::ptClip, true);
                clipper.Execute(ClipType::ctDifference, topSegment.outlinePaths);

                if (topSegment.outlinePaths.size() > 0)
                {

                    // All top segments are probably bridges
                    // TODO: implement bridge speed
                    topSegment.segmentSpeed = GlobalSettings::TravelSpeed.Get();
                    // Extrude more for a bridge
                    topSegment.infillMultiplier = 2.0f;

                    isle.segments.push_back(topSegment);
                }
            }
        }

        for (std::size_t i = layerCount - 1;
             i > layerCount - GlobalSettings::TopBottomThickness.Get() - 1; i--)
        {
            for (LayerIsland &isle : layerComponents[i].islandList)
            {
                LayerSegment topSegment(SegmentType::TopSegment);
                topSegment.outlinePaths = isle.outlinePaths;
                // All top segments are probably bridges
                // TODO: implement bridge speed
                topSegment.segmentSpeed = GlobalSettings::TravelSpeed.Get();
                // Extrude more for a bridge
                topSegment.infillMultiplier = 2.0f;

                isle.segments.push_back(topSegment);
            }
        }
    }

    // To calculate the bottom segments we need to go from the top down,
    // take each island as a subject, take the outline of the layer below
    // as a clipper and perform a difference operation. The result will
    // then be the bottom segment(s) for each layer

    // Go through every layer from the second highest layer to the second lowest layer

    if (GlobalSettings::TopBottomThickness.Get() > 0) // TODO: bottom
    {
        for (int i = layerCount - 2; i > GlobalSettings::TopBottomThickness.Get() - 1; i--)
        {
            if (i < 1)
                continue;

            // First we need to calculate the intersection of the bottom few layers below it
            Paths belowIntersection;

            for (int j = i - 1; j > i - 1 - GlobalSettings::TopBottomThickness.Get(); j--)
            {
                Paths combinedIsles;

                // Combine the outlines of the islands into one
                for (LayerIsland &isle : layerComponents[j].islandList)
                {
                    clipper.Clear();
                    clipper.AddPaths(combinedIsles, PolyType::ptClip, true);
                    clipper.AddPaths(isle.outlinePaths, PolyType::ptSubject, true);
                    clipper.Execute(ClipType::ctUnion, combinedIsles);
                }

                // Set the intial top if still unset
                if (belowIntersection.size() < 1)
                {
                    belowIntersection = combinedIsles;
                    continue;
                }

                clipper.Clear();
                clipper.AddPaths(belowIntersection, PolyType::ptSubject, true);
                clipper.AddPaths(combinedIsles, PolyType::ptClip, true);
                clipper.Execute(ClipType::ctIntersection, belowIntersection);
            }

            for (LayerIsland &isle : layerComponents[i].islandList)
            {
                LayerSegment bottomSegment(SegmentType::BottomSegment);
                clipper.Clear();
                clipper.AddPaths(isle.outlinePaths, PolyType::ptSubject, true);
                clipper.AddPaths(belowIntersection, PolyType::ptClip, true);
                clipper.Execute(ClipType::ctDifference, bottomSegment.outlinePaths);

                if (bottomSegment.outlinePaths.size() > 0)
                {                    
                    //All non initial layer bottom segments are probably bridges
                    // TODO: implement bridge speed
                    bottomSegment.segmentSpeed = GlobalSettings::TravelSpeed.Get();
                    // Extrude more for a bridge
                    bottomSegment.infillMultiplier = 2.0f;

                    isle.segments.push_back(bottomSegment);
                }
            }
        }

        // Every island in the bottom layer is obviously a bottom segment
        for (std::size_t i = 0; i < GlobalSettings::TopBottomThickness.Get(); i++)
        {
            for (LayerIsland &isle : layerComponents[i].islandList)
            {
                LayerSegment bottomSegment(SegmentType::BottomSegment);
                bottomSegment.outlinePaths = isle.outlinePaths;
                // Initial bottom segments should not be bridges
                bottomSegment.segmentSpeed = GlobalSettings::InfillSpeed.Get();

                isle.segments.push_back(bottomSegment);
            }
        }
    }
}

static inline void CalculateInfillSegments()
{
    SlicerLog("Calculating infill segments");

    // To calculate the segments that need normal infill we need to go through each island in each layer, we then need to subtract the
    // top or bottom segments from the outline shape polygons of the layer and we then have the segments that need normal infill

    Clipper clipper;

    for (std::size_t i = layerCount; i > 0; i--)
    {
        for (LayerIsland &isle : layerComponents[i].islandList)
        {
            clipper.Clear();

            // Add the outline shape polygons as the subject
            clipper.AddPaths(isle.outlinePaths, PolyType::ptSubject, true);

            // Then add existing segments such as top or bottom and outline as the clip
            for (LayerSegment seg : isle.segments)
            {
                // We do not want to subtract outline segments because we already use the overall outline of the island
                if (seg.type == SegmentType::OutlineSegment)
                    continue;

                clipper.AddPaths(seg.outlinePaths, PolyType::ptClip, true);
            }

            LayerSegment infillSeg(SegmentType::InfillSegment);
            infillSeg.segmentSpeed = GlobalSettings::InfillSpeed.Get();

            // We then need to perform a difference operation to determine the infill segments
            clipper.Execute(ClipType::ctDifference, infillSeg.outlinePaths);
        }
    }
}

static inline void CalculateSupportSegments()
{
    // TODO: implement this
}

static inline void CombineInfillSegmetns()
{
    SlicerLog("Combining infill segments");

    // To combine the infill segments we have to go through each layer, for each infill segment int that layer we need to perform
    // an intersection test with all the above layers infill segments. The combined segments should then be separated from its original
    // segments in both the first and the second layer, the same operation should then be performed with the result and all the infill
    // segments above it untill it has been done for the amount of layers specified int global values. The bottom one of the infill segment
    // will be completely removed from its layer. The extrusion multiplier of each segment will be determined by how many layers of infill
    // it represents

    std::size_t combCount = GlobalSettings::InfillCombinationCount.Get();
    if (combCount < 2)
        return;

    Clipper clipper;

    for (std::size_t i = layerCount - 1; i > 0; i -= combCount)
    {
        Paths belowInfill;

        //This will indicate how many layers have been combined
        short combinedLayerCount = 1;

        // Then figure out to what extent it intersects with the layers below
        for (std::size_t j = i - 1; j > i - combCount && j > 0; j--)
        {
            // Fist combine all the infill segments for the layer
            Paths combinedInfill;

            for (LayerIsland &isle : layerComponents[i].islandList)
            {
                for (LayerSegment &seg : isle.segments)
                {
                    if (seg.type != SegmentType::InfillSegment)
                        continue;

                    clipper.Clear();
                    clipper.AddPaths(combinedInfill, PolyType::ptSubject, true);
                    clipper.AddPaths(seg.outlinePaths, PolyType::ptClip, true);
                    clipper.Execute(ClipType::ctUnion, combinedInfill);
                }
            }

            if (belowInfill.size() > 0)
            {
                // Then determine which part intersects with the previous layer's infill
                clipper.Clear();
                clipper.AddPaths(belowInfill, PolyType::ptClip, true);
                clipper.AddPaths(combinedInfill, PolyType::ptSubject, true);
                clipper.Execute(ClipType::ctIntersection, combinedInfill);
            }
            else
                belowInfill = combinedInfill;

            combinedLayerCount++;
        }

        for (LayerIsland &mainIsle : layerComponents[i].islandList)
        {
            Paths commonInfill;

            // Start by setting all the infill in the current island as the base
            for (LayerSegment &layerSeg : mainIsle.segments)
            {
                if (layerSeg.type != SegmentType::InfillSegment)
                    continue;

                clipper.Clear();
                clipper.AddPaths(commonInfill, PolyType::ptClip, true);
                clipper.AddPaths(layerSeg.outlinePaths, PolyType::ptSubject, true);
                clipper.Execute(ClipType::ctUnion, commonInfill);
            }

            // Then determine which part intersects with the previous layer's infill
            clipper.Clear();
            clipper.AddPaths(commonInfill, PolyType::ptClip, true);
            clipper.AddPaths(belowInfill, PolyType::ptSubject, true);
            clipper.Execute(ClipType::ctIntersection, commonInfill);

            if (commonInfill.size() < 1)
                continue;

            // We should now subtract the common infill from all the affected layers and then add it to the topmost layer
            // again but with it thickness that will account for all the layers

            for (std::size_t j = i; j > i - combCount && j > 0; j--)
            {
                for (LayerIsland &isle : layerComponents[j].islandList)
                {
                    for (LayerSegment &seg : isle.segments)
                    {
                        if (seg.type != SegmentType::InfillSegment)
                            continue;

                        // Remove the common infill from the layersegment
                        clipper.Clear();
                        clipper.AddPaths(seg.outlinePaths, PolyType::ptSubject, true);
                        clipper.AddPaths(commonInfill, PolyType::ptClip, true);
                        clipper.Execute(ClipType::ctDifference, seg.outlinePaths);
                    }
                }
            }

            // Finally add the new segment to the topmost layer
            if (commonInfill.size() < 1)
                continue;

            mainIsle.segments.emplace_back(SegmentType::InfillSegment);
            LayerSegment &infillSegment = mainIsle.segments.back();
            infillSegment.infillMultiplier = combCount; // TODO: maybe different variable
            infillSegment.segmentSpeed = GlobalSettings::InfillSpeed.Get();
            infillSegment.outlinePaths = commonInfill;
        }
    }
}

static inline void GenerateRaft()
{
    // TODO: implement this
    SlicerLog("Generating raft");
}

static inline void GenerateSkirt()
{
    //  TODO: implement this
    SlicerLog("Generating skirt");
}

static void ClipLinesToPaths(std::vector<LineSegment> &lines, Paths &gridLines, Paths &paths)
{
    Clipper clipper;
    PolyTree result;
    clipper.AddPaths(gridLines, PolyType::ptSubject, false);
    clipper.AddPaths(paths, PolyType::ptClip, true);
    clipper.Execute(ClipType::ctIntersection, result);

    for (PolyNode *node : result.Childs)
    {
        if (node->Contour.size() == 2)
        {
            // Make sure the infill line is longer than at least double the nozzlewidth
            if (SquaredDist(node->Contour[0], node->Contour[1]) > NozzleWidth * scaleFactor * scaleFactor * 2)
                lines.emplace_back(node->Contour[0], node->Contour[1]);
        }
    }
}

static inline void TimInfill()
{
    SlicerLog("Trimming infill");

    bool right = false;

    for (std::size_t i = 0; i < layerCount; i++)
    {
        for (LayerIsland &isle : layerComponents[i].islandList)
        {
            for (LayerSegment &seg : isle.segments)
            {
                if (seg.type == SegmentType::InfillSegment)
                {
                    // If the segment is an infill segment then we need to trim the correlating infill grid to fill it
                    const float density = 15.0f; // TODO

                    ClipLinesToPaths(seg.fillLines, (right) ? InfillGridMap[density].rightList :
                                                              InfillGridMap[density].leftList, seg.outlinePaths);
                    seg.fillDensity = density;
                }
                else if (seg.type == SegmentType::BottomSegment || seg.type == SegmentType::TopSegment)
                {
                    // If this is a top or bottom segment then we need to trim the solid infill grid to fill it
                    ClipLinesToPaths(seg.fillLines, (right) ? InfillGridMap[1.0f].rightList :
                                                              InfillGridMap[1.0f].leftList, seg.outlinePaths);
                    seg.fillDensity = 1.0f;
                }
                else if (seg.type == SegmentType::SupportSegment)
                {
                    // If this is a support segment then we need to trim the support infill grid to fill it
                    const float density = 10.0f; // TODO
                    ClipLinesToPaths(seg.fillLines, InfillGridMap[density].leftList, seg.outlinePaths);
                    seg.fillDensity = density;
                }
                else
                    continue;
            }
        }

        right = !right;
    }
}


static inline void CalculateToolpath()
{
    SlicerLog("Calculating toolpath");

    IntPoint lastPoint(0, 0);
    cInt lastZ = 0;

    for (std::size_t i = 0; i < layerCount; i++)
    {
        bool firstInLayer = true;
        LayerComponent &curLayer = layerComponents[i];

        // Move to the new z position
        cInt newZ = lastZ + GlobalSettings::LayerHeight.Get();
        IntPoint3 p1(lastPoint.X, lastPoint.Y, lastZ);
        IntPoint3 p2(lastPoint.X, lastPoint.Y, lastZ);
        curLayer.initialLayerMoves.emplace_back(p1, p2, curLayer.layerSpeed);
        lastZ = newZ;

        for (LayerIsland &isle : curLayer.islandList)
        {
            bool firstInIsle = true;

            // The outline segments of an island should also come before all infill type segments,
            // we can therefore store the oultine
            // linesegments for use of the infill move calculations

            typedef std::vector<LineSegment> LineList;
            std::vector<LineList> outlineSegments;

            for (LayerSegment &seg : isle.segments)
            {
                // We need to convert all the polygons in the segment into lines so that we can do our calculations
                LineList lineList;
                if (seg.type == SegmentType::OutlineSegment || seg.type == SegmentType::SkirtSegment)
                {
                    // This segment contains its linesegments in its outline polygons

                    for (Path &path : seg.outlinePaths)
                    {
                        if (path.size() < 3)
                            continue;

                        for (std::size_t i = 0; i < path.size() - 1; i++)
                            lineList.emplace_back(path[i], path[i + 1]);

                        lineList.emplace_back(path.back(), path.front());
                    }
                }
                else
                {
                    // This segment contains its linesegments in its fill polygons
                    lineList.reserve(lineList.size() + seg.fillLines.size());
                    lineList.insert(lineList.end(), seg.fillLines.begin(), seg.fillLines.end());
                }

                // Store the outline linesegments for later use
                if (seg.type == SegmentType::OutlineSegment)
                    outlineSegments.push_back(lineList);

                // Move to the next segment if this one does not have any lines
                if (lineList.size() < 1)
                    continue;

                // We now have to determine if we still need to move to the island
                if (firstInIsle && !firstInLayer)
                {
                    // If we still have to move to this island then we should retract filament if needed and then create a
                    // direct move to the first point of the segment

                    if (GlobalSettings::RetractionSpeed.Get() > 0 && GlobalSettings::RetractionDistance.Get() > 0)
                    {
                        // Create a retraction because we are now moving to a new island only if we have moved more than the miniumum distance (5mm)
                        const cInt minDist = 10 * scaleFactor;
                        const cInt minDist2 = minDist * minDist;

                        //if (SquaredDist(lastPoint, lineList[0].p1) > minDist2)

                    }
                }
            }
        }
    }
}

static inline void StoreGCode(std::string outFilePath)
{
    //  TODO: implement this
    SlicerLog("Storing GCode");
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
    GenerateInfillGrids();

    // The top and bottom segments need to calculated before
    // the infill outlines otherwise the infill will be seen as top or bottom
    // Calculate the top and bottom segments
    CalculateTopBottomSegments();

    // Calculate the infill segments
    CalculateInfillSegments();

    // Calculate the support segments
    CalculateSupportSegments();

    // Combine the infill segments
    CombineInfillSegmetns();

    // Generate a raft
    GenerateRaft();

    // Generate a skirt
    GenerateSkirt();

    // Tim the infill grids to fit the segments


    // Calculate the toolpath

    // Write the toolpath as gcode
    StoreGCode(outputFile);

    // Free the memory
    if (layerComponents != nullptr)
        free(layerComponents);
}
