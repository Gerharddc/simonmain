#include "gcodeimporting.h"

#include <fstream>
#include <string>
#include <cstring>

#include "structures.h"

#include <QDebug>

Toolpath* GCodeImporting::ImportGCode(const char *path)
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

        Layer *curLayer = nullptr;
        int8_t g = -1;
        bool rel = false;

        // TODO: add support for comments
        // TODO: add support for home command
        // TODO: add support for set pos command

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
                    if (type == SetRel)
                        rel = true;
                    else if (type == SetAbs)
                        rel = false;
                    else if (type == Home)
                    {
                        prevX[0] = 0;
                        prevX[1] = 0;
                        prevY[0] = 0;
                        prevY[1] = 0;
                        prevZ[0] = 0;
                        prevZ[1] = 0;
                    }

                    continue;
                }
            }

            bool moved = false; // Flag if movement occured

            // Create a point at the last position
            Point p;
            p.extruded = false;

            // Read the parts as spilt by spaces
            // TODO: add support or at least warnings for irregularly long whitespaces
            // Working correctly with the c_str  directly in strtok
            // breaks the std::string a bit and complicates debugging
#define PRESERVE_LINE_STR
#ifdef PRESERVE_LINE_STR
            auto temp = line.c_str();
            auto len = strlen(temp);
            char *lineCpy = new char[len];
            memcpy(lineCpy, temp, sizeof(char) * len);
            char *pch = std::strtok(lineCpy, " ");
#else
            char *pch = std::strtok((char*)line.c_str(), " ");
#endif

            while (pch != NULL)
            {
                std::string part(pch);
                char c = part[0];
                auto numLen = strlen(pch); // Keep the null termination (-1 + 1)
                char numS[numLen]; // Buffer for number
                memcpy(numS, &pch[1], numLen);
                float num = std::atof(numS);

                pch = std::strtok(NULL, "     "); // Get the next part so long

                switch(c) {
                    case 'X' :
                    {
                        float old = prevX[g];

                        if (rel)
                            prevX[g] += num;
                        else
                            prevX[g] = num;

                        if (!moved && (prevX[g] != old))
                        {
                            moved = true;
                        }
                        break;
                    }
                    case 'Y' :
                    {
                        float old = prevY[g];

                        if (rel)
                            prevY[g] += num;
                        else
                            prevY[g] = num;

                        if (!moved && (prevY[g] != old))
                        {
                            moved = true;
                        }
                        break;
                    }
                    case 'Z' :
                    {
                        float nZ = (rel) ? (prevZ[g] + num) : num;

                        if (nZ != prevZ[g])
                        {
                            // Create a new layer when moving to a new z
                            // and optimize the old one
                            if (curLayer != nullptr)
                                curLayer->points.shrink_to_fit();

                            tp->layers.emplace_back();
                            curLayer = &(tp->layers.back());
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
            }
#ifdef PRESERVE_LINE_STR
            delete[] lineCpy;
#endif

            // Skip the line if there was no movement
            if (!moved)
            {
                continue;
            }

            // Wait until we have current values to work from
            if (!prevValid[g])
            {
                prevValid[g] = (prevX[g] != -1) && (prevY[g] != -1) && (prevZ[g] != -1);

                if (!prevValid[g])
                    continue;
            }

            // Assign the position
            p.x = prevX[g];
            p.y = prevY[g];

            if (curLayer != nullptr)
                curLayer->points.push_back(p);
        }

        if (curLayer != nullptr)
            curLayer->points.shrink_to_fit();
    }

    return tp;
}
