#include "stlexporting.h"

#include <fstream>

namespace STLExporting {
    bool ExportSTL(std::string path, const std::set<Mesh *> meshes, std::string &error)
    {
        std::ofstream os(path, std::ofstream::out | std::ofstream::binary);
        // TODO: check for errors
        if (os.is_open())
        {
            // Write the header
            char header[80];
#define HEADERTEXT "Written by naai software..."
            const char *headerText = HEADERTEXT;
            memcpy(header, headerText, sizeof HEADERTEXT);
            os << header;

            // Write the size
            uint32_t trigCount = 0;
            for (Mesh *mesh : meshes)
                trigCount += mesh->trigCount;
            os.write((char*)&trigCount, 4);

            // Write the triangles
            for (Mesh *mesh : meshes)
            {
                // Get the vertex and normal floats
                const float *verts = mesh->getFlatVerts();
                const float *norms = mesh->getFlatNorms();

                for (std::size_t i = 0; i < mesh->trigCount; i++)
                {
                    // Write the normal first
                    os.write((char*)norms, 4);
                    norms += 4;

                    // Write the three vertices
                    for (uint8_t j = 0; j < 3; j++)
                    {
                        os.write((char*)verts, 4);
                        verts += 4;
                    }
                }

                // Dump the temporary arrays
                mesh->dumpFlatVerts();
                mesh->dumpFlatNorms();
            }

            os.close();
        }
        else
            error = "Could not open file";

        return false;
    }
}
