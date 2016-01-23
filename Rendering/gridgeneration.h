#ifndef GRIDGENERATION_H
#define GRIDGENERATION_H

#include <cstdlib>

namespace GridGeneration {

    typedef unsigned int uint;

    float* GenerateGrids(float xSize, float ySize, float zSize, float interval)
    {
        uint bottomXCount = std::div(xSize, interval);
        uint bottomYCount = std::div(ySize, interval);
        float bottomXInt = xSize / bottomXCount;
        float bottomYInt = ySize / bottomYCount;

        // Each line has 2 points and each point has 3 floats (x,y,z)
        float* buf = new float[(bottomXCount + bottomYCount) * 6];

        uint idx = 0;

        // Calculate the vertical bottom lines
        for (uint i = 0; i < bottomXCount; i++)
        {
            float x = bottomXInt * i;

            buf[idx    ] = x; // x1
            buf[idx + 1] = 0; // y1
            buf[idx + 2] = 0; // z1

            buf[idx + 3] = x;     // x2
            buf[idx + 4] = ySize; // y2
            buf[idx + 5] = 0;     // z2

            idx += 6;
        }

        // Calculate the horizontal bottom lines
        for (uint i = 0; i < bottomYCount; i++)
        {
            float y = bottomYInt * i;

            buf[idx    ] = 0; // x1
            buf[idx + 1] = y; // y1
            buf[idx + 2] = 0; // z1

            buf[idx + 3] = xSize; // x2
            buf[idx + 4] = y;     // y2
            buf[idx + 5] = 0;     // z2

            idx += 6;
        }

        return buf;
    }

}

#endif // GRIDGENERATION_H
