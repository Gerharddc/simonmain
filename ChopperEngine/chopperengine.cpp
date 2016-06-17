#include "chopperengine.h"
#include "Misc/globalsettings.h"
#include <iostream>

std::size_t ChopperEngine::layerCount = 0;
Mesh* ChopperEngine::sliceMesh = nullptr;
static ChopperEngine::LogDelegate slicerLogger = nullptr;

void ChopperEngine::SlicerLog(std::string message)
{
    if (slicerLogger != nullptr)
        slicerLogger(message);

    std::cout << message << std::endl;
}

void ChopperEngine::SliceFile(Mesh *inputMesh, std::string outputFile)
{
    sliceMesh = inputMesh;

    // Calculate the amount layers that will be sliced
    layerCount = sliceMesh->MaxVec.z / GlobalSettings::LayerHeight;

    // Slice the triangles into layers

    // Calculate islands from the original lines

    // Optimize the outline polygons

    // Generate the outline segments

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
}
