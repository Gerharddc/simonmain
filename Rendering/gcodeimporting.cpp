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
        float prevX = -1;
        float prevY = -1;
        float prevZ = -1;
        float prevF[2] = {-1, -1};
        float prevE = 0;
        bool prevValid = false;

        Layer *curLayer = nullptr;
        Island *curIsle = nullptr;
        Point3 lastPoint;

        int8_t g = -1;
        bool rel = false;
        bool setPos = false;

        // Keep track if the last action was a move or
        // an extrusion
        bool lastWasMove = true;

        // TODO: add support for set pos command

        // Read through each line
        std::string line;
        while (std::getline(is, line))
        {
            bool extruded = false;
            int type = -1;

            // Remove all comments from the line
            line = line.substr(0, line.find(';') - 1);

            //qDebug() << QString(line.c_str());

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
                else if (type == SetPos)
                {
                    // TODO: maybe wrong
                    prevX = 0;
                    prevY = 0;
                    prevZ = 0;
                    prevE = 0;
                    setPos = true;
                }
                else
                {
                    if (type == SetRel)
                        rel = true;
                    else if (type == SetAbs)
                        rel = false;
                    else if (type == Home)
                    {
                        prevX = 0;
                        prevY = 0;
                        prevZ = 0;
                    }

                    continue;
                }
            }

            bool moved = false; // Flag if movement occured

            // Create a point at the last position
            lastPoint = { prevX, prevY, prevZ };

            // Read the parts as spilt by spaces
            // TODO: add support or at least warnings for irregularly long whitespaces
            // Working correctly with the c_str  directly in strtok
            // breaks the std::string a bit and complicates debugging
//#define PRESERVE_LINE_STR
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
                        float old = prevX;

                        if (rel)
                            prevX += num;
                        else
                            prevX = num;

                        if (!moved && (prevX != old))
                        {
                            moved = true;
                        }
                        break;
                    }
                    case 'Y' :
                    {
                        float old = prevY;

                        if (rel)
                            prevY += num;
                        else
                            prevY = num;

                        if (!moved && (prevY != old))
                        {
                            moved = true;
                        }
                        break;
                    }
                    case 'Z' :
                    {
                        float nZ = (rel) ? (prevZ + num) : num;

                        if (nZ != prevZ)
                        {
                            // Create a new layer when moving to a new z
                            // and optimize the old one
                            if (curLayer != nullptr)
                                curLayer->islands.shrink_to_fit();

                            tp->layers.emplace_back();
                            curLayer = &(tp->layers.back());
                            curLayer->z = num;

                            if (curIsle != nullptr)
                            {
                                curIsle->movePoints.shrink_to_fit();
                                curIsle->printPoints.shrink_to_fit();
                            }

                            curLayer->islands.emplace_back();
                            lastWasMove = true;
                            curIsle = &(curLayer->islands.back());
                            curIsle->movePoints.push_back(lastPoint);

                            moved = true;
                        }

                        prevZ = num;
                        break;
                    }
                    case 'F' :
                        if (rel)
                            prevF[g] += num;
                        else
                            prevF[g] = num;
                        break;
                    case 'E' :
                        float nZ = (rel) ? (prevZ + num) : num;

                        // TODO: detect retractions
                        if (prevE != nZ)
                            extruded = true;
                        break;
                }
            }
#ifdef PRESERVE_LINE_STR
            delete[] lineCpy;
#endif

            // Skip the line if there was no movement
            if (!moved || setPos)
            {
                continue;
            }

            // Wait until we have current values to work from
            if (!prevValid)
            {
                prevValid = (prevX != -1) && (prevY != -1) && (prevZ != -1);

                if (!prevValid)
                    continue;
            }

            // TODO: points get lost when not on layers
            if (curLayer == nullptr)
                continue;

            if (extruded)
            {
                curIsle->printPoints.push_back({prevX, prevY});

                lastWasMove = false;
            }
            else
            {
                if (!lastWasMove)
                {
                    if (curIsle != nullptr)
                    {
                        curIsle->movePoints.shrink_to_fit();
                        curIsle->printPoints.shrink_to_fit();
                    }

                    // If the last action was not a move, then we are now starting a new island
                    curLayer->islands.emplace_back();
                    curIsle = &(curLayer->islands.back());

                    curIsle->movePoints.push_back(lastPoint);
                }

                curIsle->movePoints.push_back({prevX, prevY, prevZ});

                lastWasMove = true;
            }
        }

        if (curLayer != nullptr)
            curLayer->islands.shrink_to_fit();

        if (curIsle != nullptr)
        {
            curIsle->movePoints.shrink_to_fit();
            curIsle->printPoints.shrink_to_fit();
        }
    }

    return tp;
}
