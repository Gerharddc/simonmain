#ifndef GRIDGENERATION_H
#define GRIDGENERATION_H

#include <cstdlib>

namespace GridGeneration {

    struct Grid
    {
        float* floats;
        std::size_t floatCount;

        Grid(std::size_t cnt)
        {
            floats = new float[cnt];
            floatCount = cnt;
        }

        ~Grid()
        {
            delete[] floats;
        }
    };

#define uint unsigned int

    inline Grid* GenerateGrids(uint xSize, uint ySize, uint zSize, uint interval)
    {   
        // When calculate the amount of blocks and then add the final line
        uint bottomXCount = (xSize / interval);
        uint bottomYCount = (ySize / interval);
        float bottomXInt = xSize / bottomXCount;
        float bottomYInt = ySize / bottomYCount;
        bottomXCount++;
        bottomYCount++;

        // Each line has 2 points and each point has 3 floats (x,y,z)
        Grid *grid = new Grid((bottomXCount + bottomYCount) * 6);

        uint idx = 0;

        // Calculate the vertical bottom lines
        for (uint i = 0; i < bottomXCount; i++)
        {
            float x = bottomXInt * i;

            grid->floats[idx    ] = x; // x1
            grid->floats[idx + 1] = 0; // y1
            grid->floats[idx + 2] = 0; // z1

            grid->floats[idx + 3] = x;     // x2
            grid->floats[idx + 4] = ySize; // y2
            grid->floats[idx + 5] = 0;     // z2

            idx += 6;
        }

        // Calculate the horizontal bottom lines
        for (uint i = 0; i < bottomYCount; i++)
        {
            float y = bottomYInt * i;

            grid->floats[idx    ] = 0; // x1
            grid->floats[idx + 1] = y; // y1
            grid->floats[idx + 2] = 0; // z1

            grid->floats[idx + 3] = xSize; // x2
            grid->floats[idx + 4] = y;     // y2
            grid->floats[idx + 5] = 0;     // z2

            idx += 6;
        }

        return grid;
    }

}

#endif // GRIDGENERATION_H
