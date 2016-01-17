#include "gcodeimporting.h"

#include <fstream>
#include <string>
#include <cstring>

#include "structures.h"

namespace GCodeImportent {

Toolpath* ImportGCode(const char *path)
{
    std::ifstream is(path);

    Toolpath *tp = new Toolpath();

    // Check for a valid file
    if (is)
    {
        // Store the previous values for G0 and G1
        float prevX[2] = {-1, -1};
        float prevY[2] = {-1, -1};
        float prevZ[2] = {-1, -1};
        float prevF[2] = {-1, -1};
        float prevE[2] = {0, 0};
        bool prevValid[2] = {false, false};

        Layer *curLayer = NULL;
        int8_t g = -1;
        bool rel = false;

        // Read through each line
        std::string line;
        while (std::getline(is, line))
        {
            int type = -1;

            // Confirm the line starts wit g and get the code
            if (line[0] != 'G')
                continue;
            else
            {
                auto pos = line.find(' ');
                std::string code = line.substr(1, pos - 1);
                type = std::stoi(code);

                if (type == 0 || type == 1)
                    g = type;
                else
                {
                    g = -1;

                    if (type == SetRel)
                        rel = true;
                    else if (type == SetAbs)
                        rel = false;
                }
            }

            // Read the parts as spilt by spaces
            // TODO: add support or at least warnigns for irregularly long whitespaces
            char *pch = std::strtok((char*)line.c_str(), "     ");
            while (pch != NULL)
            {
                std::string part(pch);
                char c = part[0];
                auto numLen = strlen(pch); // Keep the null termination (-1 + 1)
                char numS[numLen]; // Buffer for number
                memcpy(numS, &pch[1], numLen);
                float num = std::atof(numS);

                pch = std::strtok(NULL, "     "); // Get the next part so long

                Point p;
                p.x = prevX[g];
                p.y = prevY[g];
                p.extruded = false;

                switch(c) {
                    case 'X' :
                        if (rel)
                            prevX[g] += num;
                        else
                            prevX[g] = num;
                        break;
                    case 'Y' :
                        if (rel)
                            prevY[g] += num;
                        else
                            prevY[g] = num;
                        break;
                    case 'Z' :
                    {
                        float nZ = (rel) ? (prevZ[g] + num) : num;

                        if (nZ != prevZ[g])
                        {
                            if (curLayer != NULL)
                                tp->layers.push_back(curLayer);
                            curLayer = new Layer();
                            curLayer->z = num;
                        }

                        prevZ[g] = num;
                        break;
                    }
                    case 'F' :
                        if (rel)
                            prevF[g] += num;
                        else
                            prevF[g] = num;
                        break;
                    case 'E' :
                        float nZ = (rel) ? (prevZ[g] + num) : num;

                        // TODO: detect retractions
                        if (prevE[g] != nZ)
                            p.extruded = true;
                        break;
                }

                // Wait until we have current values to work from
                if (!prevValid[g])
                {
                    prevValid[g] = (prevX[g] != -1) && (prevY[g] != -1) && (prevZ[g] != -1);

                    if (!prevValid[g])
                        continue;
                }

                curLayer->points.push_back(p);
            }
        }
    }
}

}
